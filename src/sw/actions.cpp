#include "actions.hpp"

#include "../IO/Events/MarchEnded.hpp"
#include "../IO/Events/UnitAttacked.hpp"
#include "../IO/Events/UnitDied.hpp"
#include "../IO/Events/UnitMoved.hpp"
#include "unitRepository.hpp"
#include "units.hpp"

#include <algorithm>
#include <limits>

namespace sw::actions
{

bool RangeAttack::tryRangeAttack(units::Unit* unit)
{
    pos_t pos = unit->getPosition();

    // Find the closest, weakest, or lowest ID target within range
    std::optional<uint32_t> targetId;
    uint32_t minDistance = range + 1;
    uint32_t minHp = std::numeric_limits<uint32_t>::max();
    uint32_t minId = std::numeric_limits<uint32_t>::max();
    auto& map = unit->getRepo().getMap();

    for (uint32_t x = pos.x - range; x <= pos.x + range; ++x)
    {
        for (uint32_t y = pos.y - range; y <= pos.y + range; ++y)
        {
            if (x < map.getWidth() && y < map.getHeight() && map.getCell({x, y}) != unit->getId() && map.isCellOccupied({x, y}) &&
                unit->getRepo().getUnit(map.getCell({x, y}))->isAlive() && abs(x - pos.x) > 1 && abs(y - pos.y) > 1)
            {
                uint32_t currentTargetId = map.getCell({x, y});
                uint32_t distance = std::max(abs(x - pos.x), abs(y - pos.y));

                if (distance <= range && distance < minDistance)
                {
                    targetId = currentTargetId;
                    minDistance = distance;
                    minHp = unit->getRepo().getUnit(*targetId)->getHp();
                    minId = *targetId;
                }
                else if (distance == minDistance && unit->getRepo().getUnit(currentTargetId)->getHp() < minHp)
                {
                    targetId = currentTargetId;
                    minHp = unit->getRepo().getUnit(*targetId)->getHp();
                    minId = *targetId;
                }
                else if (distance == minDistance && unit->getRepo().getUnit(currentTargetId)->getHp() == minHp && currentTargetId < minId)
                {
                    targetId = currentTargetId;
                    minId = *targetId;
                }
            }
        }
    }

    if (targetId.has_value())
    {
        auto* target = unit->getRepo().getUnit(*targetId);
        uint32_t targetHp = target->getHp() > agility ? target->getHp() - agility : 0;
        target->setHP(targetHp);
        log(tick, io::UnitAttacked{unit->getId(), *targetId, agility, targetHp});
        if (!target->isAlive()) { log(tick, io::UnitDied{*targetId}); }
        return true;
    }

    return false;
}

bool MeleeAttack::tryMeleeAttack(units::Unit* unit)
{
    pos_t pos = unit->getPosition();

    // Check adjacent cells
    std::vector<std::pair<int, int>> adjacentCells = {
        {pos.x - 1, pos.y - 1}, {pos.x, pos.y - 1}, {pos.x + 1, pos.y - 1}, {pos.x - 1, pos.y},
        {pos.x - 1, pos.y + 1}, {pos.x + 1, pos.y}, {pos.x + 1, pos.y + 1}, {pos.x, pos.y + 1}};

    // Find the weakest or lowest ID target within melee range
    std::optional<uint32_t> targetId;
    uint32_t minHp = std::numeric_limits<uint32_t>::max();
    uint32_t minId = std::numeric_limits<uint32_t>::max();
    auto& map = unit->getRepo().getMap();
    for (auto& cell : adjacentCells)
    {
        uint32_t x = cell.first;
        uint32_t y = cell.second;
        if (x < map.getWidth() && y < map.getHeight() && map.isCellOccupied({x, y}) && unit->getRepo().getUnit(map.getCell({x, y}))->isAlive())
        {
            uint32_t currentTargetId = map.getCell({x, y});
            if (unit->getRepo().getUnit(currentTargetId)->getHp() < minHp)
            {
                targetId = currentTargetId;
                minHp = unit->getRepo().getUnit(*targetId)->getHp();
                minId = *targetId;
            }
            else if (unit->getRepo().getUnit(currentTargetId)->getHp() == minHp && currentTargetId < minId)
            {
                targetId = currentTargetId;
                minId = *targetId;
            }
        }
    }

    if (targetId.has_value())
    {
        auto* target = unit->getRepo().getUnit(*targetId);
        uint32_t targetHp = target->getHp() > strength ? target->getHp() - strength : 0;
        target->setHP(targetHp);
        log(tick, io::UnitAttacked{unit->getId(), *targetId, strength, targetHp});
        if (!target->isAlive()) { log(tick, io::UnitDied{*targetId}); }
        return true;
    }

    return false;
}

bool Move::tryMove(units::Unit* unit)
{
    if (destination.has_value())
    {
        auto& map = unit->getRepo().getMap();
        pos_t pos = unit->getPosition();
        pos.x = pos.x + 1 * (destination->x > pos.x) - 1 * (destination->x < pos.x);
        pos.y = pos.y + 1 * (destination->y > pos.y) - 1 * (destination->y < pos.y);

        if (!map.isCellOccupied(pos))
        {
            map.freeCell(unit->getPosition());
            map.occupyCell(pos, unit->getId());
            unit->setPosition(pos);
            log(tick, io::UnitMoved{unit->getId(), pos.x + 1, pos.y + 1});

            if (pos == *destination)
            {
                destination.reset();
                log(tick, io::MarchEnded{unit->getId(), pos.x + 1, pos.y + 1});
            }

            return true;
        }

        return false;
    }

    return false;
}

} // namespace sw::actions
