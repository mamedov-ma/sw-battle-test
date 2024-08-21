#pragma once

#include "units.hpp"

#include <memory>

namespace sw::units
{

class UnitFactory
{
public:
    static std::unique_ptr<Unit> createWarrior(UnitRepository& unitRepo, int id, int hp, pos_t pos, int strength)
    {
        return std::make_unique<Warrior>(unitRepo, id, hp, pos, strength);
    }

    static std::unique_ptr<Unit> createArcher(UnitRepository& unitRepo, int id, int hp, pos_t pos, int strength, int range, int agility)
    {
        return std::make_unique<Archer>(unitRepo, id, hp, pos, strength, range, agility);
    }
};

} // namespace sw::units
