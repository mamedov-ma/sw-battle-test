#pragma once

#include "../IO/Events/MarchStarted.hpp"
#include "actions.hpp"
#include <string>

using namespace std::string_literals;

namespace sw::units
{

class UnitRepository;

class Unit : public actions::Move
{
public:
    explicit Unit(UnitRepository& unitRepo, uint32_t id, uint32_t hp, pos_t pos)
        : unitRepo{unitRepo}
        , id{id}
        , hp{hp}
        , pos{pos}
    {}

    uint32_t getId() const noexcept { return id; }
    uint32_t getHp() const noexcept { return hp; }
    pos_t getPosition() const noexcept { return pos; }
    UnitRepository& getRepo() { return unitRepo; }

    void setHP(uint32_t newHp) noexcept { hp = newHp; }
    void setPosition(pos_t newPos) noexcept { pos = newPos; }
    void setDestination(pos_t dest)
    {
        destination = dest;
        log(tick, io::MarchStarted{id, pos.x + 1, pos.y + 1, destination->x + 1, destination->y + 1});
    }

    bool isAlive() const noexcept { return hp > 0; }

    virtual bool tryPerformAction() = 0;
    virtual std::string getName() = 0;

    virtual ~Unit() {}

protected:
    UnitRepository& unitRepo;
    uint32_t id;
    uint32_t hp;
    pos_t pos;
};

class Warrior final : public Unit
    , public actions::MeleeAttack
{
public:
    explicit Warrior(UnitRepository& unitRepo, uint32_t id, uint32_t hp, pos_t pos, uint32_t strength)
        : Unit{unitRepo, id, hp, pos}
        , actions::MeleeAttack{strength}
    {}

    bool tryPerformAction() override
    {
        if (tryMeleeAttack(this)) { return true; }
        if (tryMove(this)) { return true; }
        return false;
    }

    std::string getName() override { return name; }

    std::string name = "warrior"s;
};

class Archer final : public Unit
    , public actions::MeleeAttack
    , actions::RangeAttack
{
public:
    explicit Archer(UnitRepository& unitRepo, uint32_t id, uint32_t hp, pos_t pos, uint32_t strength, uint32_t range, uint32_t agility)
        : Unit{unitRepo, id, hp, pos}
        , actions::MeleeAttack{strength}
        , RangeAttack{range, agility}
    {}

    bool tryPerformAction() override
    {
        if (tryRangeAttack(this)) { return true; }
        if (tryMeleeAttack(this)) { return true; }
        if (tryMove(this)) { return true; }
        return false;
    }

    std::string getName() override { return name; }

    std::string name = "archer"s;
};

} // namespace sw::units
