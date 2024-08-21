#pragma once

#include "../IO/System/details/PrintFieldVisitor.hpp"
#include "../IO/System/EventLog.hpp"
#include "map.hpp"

#include <optional>

extern uint32_t tick;

namespace sw::units
{
class Unit;
}

namespace sw::actions
{

struct RangeAttack : protected sw::EventLog
{
    explicit RangeAttack(uint32_t range, uint32_t agility)
        : range{range}
        , agility{agility}
    {}

    bool tryRangeAttack(units::Unit*);

    uint32_t range;
    uint32_t agility;
};

struct MeleeAttack : protected sw::EventLog
{
    explicit MeleeAttack(uint32_t strength)
        : strength{strength}
    {}

    bool tryMeleeAttack(units::Unit*);

    uint32_t strength;
};

struct Move : protected sw::EventLog
{
    bool tryMove(units::Unit*);

    std::optional<pos_t> destination;
};

} // namespace sw::actions
