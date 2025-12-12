#include "factory.h"
#include "toad.h"     
#include "dragon.h"
#include "knight.h"

std::shared_ptr<NPC> NPCFactory::create(NpcType type, const std::string& name, int x, int y) {
    switch (type) {
        case NpcType::Toad:    
            return std::make_shared<Toad>(name, x, y);
        case NpcType::Dragon:
            return std::make_shared<Dragon>(name, x, y);
        case NpcType::Knight:
            return std::make_shared<Knight>(name, x, y);
        default:
            return nullptr;
    }
}

std::shared_ptr<NPC> NPCFactory::create(std::istream& is) {
    std::string type, name;
    int x, y;
    
    if (is >> type >> name >> x >> y) {
        if (type == "Toad") { 
            return std::make_shared<Toad>(name, x, y);
        } else if (type == "Dragon") {
            return std::make_shared<Dragon>(name, x, y);
        } else if (type == "Knight") {
            return std::make_shared<Knight>(name, x, y);
        }
    }
    return nullptr;
}

void NPCFactory::save(const std::shared_ptr<NPC>& npc, std::ostream& os) {
    if (npc) {
        os << npc->getType() << " " << npc->getName() << " " << npc->getX() << " " << npc->getY() << "\n";
    }
}