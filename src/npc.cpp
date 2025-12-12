#include <cmath>

#include "npc.h"

NPC::NPC(const std::string& name, int x, int y) 
    : name(name), x(x), y(y), alive(true) {
    if (x < 0 || x > 500 || y < 0 || y > 500) {
        throw std::runtime_error("NPC coordinates must be in range 0-500");
    }
}

double NPC::distance(const std::shared_ptr<NPC>& other) const {
    if (!other) return 0;
    int dx = x - other->x;
    int dy = y - other->y;
    return std::sqrt(dx * dx + dy * dy);
}