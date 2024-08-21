#pragma once

#include "map.hpp"
#include "units.hpp"
#include "../IO/System/details/PrintFieldVisitor.hpp"
#include "../IO/System/EventLog.hpp"
#include "../IO/Events/UnitSpawned.hpp"


#include <memory>
#include <unordered_map>

extern uint32_t tick;

namespace sw::units
{

class UnitRepository : private sw::EventLog
{
public:
    void createMap(uint32_t width, uint32_t height) { map.create(width, height); }

    bool tryAddUnit(std::unique_ptr<Unit> unit)
    {
        uint32_t id = unit->getId();
        pos_t pos = unit->getPosition();
        auto name = unit->getName();
        auto [_, emplaced] = units.try_emplace(unit->getId(), std::move(unit));
        if(emplaced && !map.isCellOccupied(pos))
        {
            map.occupyCell(pos, id);
            log(tick, io::UnitSpawned{id, name, pos.x + 1, pos.y + 1});
        }
        return emplaced;
    }

    Unit* getUnit(uint32_t id) { return units.at(id).get(); }
    sw::Map& getMap() noexcept { return map; }

private:
    std::unordered_map<uint32_t, std::unique_ptr<Unit>> units;
    sw::Map map;
};

} // namespace sw::units
