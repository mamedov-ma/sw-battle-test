#include "sw/details/task_queue.hpp"
#include <IO/System/CommandParser.hpp>
#include <IO/System/PrintDebug.hpp>
#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/SpawnWarrior.hpp>
#include <IO/Commands/SpawnArcher.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/Wait.hpp>
#include <IO/System/EventLog.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <sw/unitRepository.hpp>
#include <sw/unitFactory.hpp>

#include <iostream>
#include <fstream>

uint32_t tick = 1;

int main(int argc, char** argv)
{
    using namespace sw;

    if (argc != 2)
    {
        throw std::runtime_error("Error: No file specified in command line argument");
    }

    std::ifstream file(argv[1]);
    if (!file)
    {
        throw std::runtime_error("Error: File not found - " + std::string(argv[1]));
    }

    std::cout << "Commands:\n";

    sw::units::UnitRepository repo;
    sw::units::UnitFactory factory;
    std::vector<std::optional<uint32_t>> ids;
    sw::details::TaskQueue taskQueue;
    io::CommandParser parser;

    parser.add<io::CreateMap>(
              [&repo, &taskQueue](auto command) {
                  taskQueue.enqueue([&repo, command]() { repo.createMap(command.width, command.height); });
                  printDebug(std::cout, command);
              })
        .add<io::SpawnWarrior>(
            [&ids, &factory, &repo, &taskQueue](auto command) {
                taskQueue.enqueue([&ids, &factory, &repo, command]() {
                    repo.tryAddUnit(factory.createWarrior(
                        repo, command.unitId, command.hp, {command.x - 1, command.y - 1}, command.strength));
                    ids.emplace_back(command.unitId);
                });
                printDebug(std::cout, command);
            })
        .add<io::SpawnArcher>(
            [&ids, &factory, &repo, &taskQueue](auto command) {
                taskQueue.enqueue([&ids, &factory, &repo, command]() {
                    repo.tryAddUnit(factory.createArcher(
                        repo, command.unitId, command.hp, {command.x - 1, command.y - 1}, command.strength, command.range, command.agility));
                    ids.emplace_back(command.unitId);
                });
                printDebug(std::cout, command);
            })
        .add<io::March>(
            [&factory, &repo, &taskQueue](auto command) {
                taskQueue.enqueue([&factory, &repo, command]() { repo.getUnit(command.unitId)->setDestination({command.targetX - 1, command.targetY - 1}); });
                printDebug(std::cout, command);
            })
        .add<io::Wait>(
            [&ids, &factory, &repo, &taskQueue](auto command) {
                taskQueue.enqueue([&ids, &factory, &repo, command]() {
                    // TODO: get rid of repeating code, perhaps introduce static lambda instead
                    std::size_t ticksAfterWait = tick + command.ticks;
                    units::Unit* unit;
                    while (tick < ticksAfterWait)
                    {
                        for (auto const& id : ids)
                        {
                            if (id.has_value())
                            {
                                unit = repo.getUnit(*id);
                                unit->tryPerformAction();
                            }
                        }

                        for (auto& id : ids)
                        {
                            if (id.has_value())
                            {
                                unit = repo.getUnit(*id);
                                if (!unit->isAlive())
                                {
                                    unit->getRepo().deleteUnit(*id);
                                    id.reset();
                                }
                            }
                        }
                        ++tick;
                    }
                });
                printDebug(std::cout, command);
            });

    parser.parse(file);

    std::cout << "\n\nEvents:\n";

    taskQueue.process();

    bool acted;
    units::Unit* unit;
    while (true)
    {
        acted = false;
        for (auto const& id : ids)
        {
            if (id.has_value())
            {
                unit = repo.getUnit(*id);
                acted = acted || unit->tryPerformAction();
            }
        }

        for (auto& id : ids)
        {
            if (id.has_value())
            {
                unit = repo.getUnit(*id);
                if (!unit->isAlive())
                {
                    unit->getRepo().deleteUnit(*id);
                    id.reset();
                }
            }
        }
        ++tick;
        if (!acted) { break; }
    }

    return 0;
}
