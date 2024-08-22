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

static units::Unit* findTarget(units::Unit* unit, uint32_t minRange, uint32_t maxRange)
{
    pos_t pos = unit->getPosition();

    // Find the closest, weakest, or lowest ID target within range
    std::optional<uint32_t> targetId;
    uint32_t minDistance = maxRange + 1;
    uint32_t minHp = std::numeric_limits<uint32_t>::max();
    uint32_t minId = std::numeric_limits<uint32_t>::max();
    auto& map = unit->getRepo().getMap();

    for (uint32_t x = pos.x - maxRange; x <= pos.x + maxRange; ++x)
    {
        for (uint32_t y = pos.y - maxRange; y <= pos.y + maxRange; ++y)
        {
            if (x < map.getWidth() && y < map.getHeight() && map.getCell({x, y}) != unit->getId() && map.isCellOccupied({x, y}) &&
                unit->getRepo().getUnit(map.getCell({x, y}))->isAlive() && abs(x - pos.x) >= minRange && abs(y - pos.y) >= minRange)
            {
                uint32_t currentTargetId = map.getCell({x, y});
                uint32_t distance = std::max(abs(x - pos.x), abs(y - pos.y));

                if (distance <= maxRange && distance < minDistance)
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

    if (targetId.has_value()) { return unit->getRepo().getUnit(*targetId); }
    return nullptr;
}

bool RangeAttack::tryRangeAttack(units::Unit* unit)
{
    if (auto* target = findTarget(unit, 2, range); target)
    {
        uint32_t targetHp = target->getHp() > agility ? target->getHp() - agility : 0;
        target->setHP(targetHp);
        log(tick, io::UnitAttacked{unit->getId(), target->getId(), agility, targetHp});
        if (!target->isAlive()) { log(tick, io::UnitDied{target->getId()}); }
        return true;
    }

    return false;
}

bool MeleeAttack::tryMeleeAttack(units::Unit* unit)
{
    if (auto* target = findTarget(unit, 0, 1); target)
    {
        uint32_t targetHp = target->getHp() > strength ? target->getHp() - strength : 0;
        target->setHP(targetHp);
        log(tick, io::UnitAttacked{unit->getId(), target->getId(), strength, targetHp});
        if (!target->isAlive()) { log(tick, io::UnitDied{target->getId()}); }
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
