#pragma once

#include "../IO/Events/MapCreated.hpp"
#include "../IO/System/details/PrintFieldVisitor.hpp"
#include "../IO/System/EventLog.hpp"

#include <optional>
#include <cstdint>
#include <vector>

extern uint32_t tick;

namespace sw
{

struct pos_t
{
    bool operator==(pos_t const& rhs) const = default;
    uint32_t x;
    uint32_t y;
};

class Map : private sw::EventLog
{
public:
    void create(uint32_t width, uint32_t height)
    {
        this->width = width;
        this->height = height;
        cells.resize(width);
        for (auto& row : cells) { row.resize(height, std::nullopt); }
        log(tick, io::MapCreated{width, height});
    }

    uint32_t getCell(pos_t pos) const { return *cells[pos.x][pos.y]; }
    bool isCellOccupied(pos_t pos) const { return cells[pos.x][pos.y].has_value(); }

    void occupyCell(pos_t pos, uint32_t id) { cells[pos.x][pos.y] = id; }
    void freeCell(pos_t pos) { cells[pos.x][pos.y].reset(); }

    uint32_t getWidth() const { return width; }
    uint32_t getHeight() const { return height; }

private:
    std::vector<std::vector<std::optional<uint32_t>>> cells;
    uint32_t width;
    uint32_t height;
};

} // namespace sw
