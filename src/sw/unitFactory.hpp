#pragma once

#include "units.hpp"

#include <memory>

namespace sw::units
{

class UnitFactory
{
public:
    static std::unique_ptr<Unit> createWarrior(UnitRepository& unitRepo, int32_t id, int32_t hp, pos_t pos, int32_t strength)
    {
        return std::make_unique<Warrior>(unitRepo, id, hp, pos, strength);
    }

    static std::unique_ptr<Unit> createArcher(UnitRepository& unitRepo, int32_t id, int32_t hp, pos_t pos, int32_t strength, int32_t range, int32_t agility)
    {
        return std::make_unique<Archer>(unitRepo, id, hp, pos, strength, range, agility);
    }
};

} // namespace sw::units
