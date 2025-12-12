#pragma once

#include "npc.h"
#include "fightVisitor.h"

class Dragon : public NPC, public std::enable_shared_from_this<Dragon> {
public:
    Dragon(const std::string& name, int x, int y);
    
    bool accept(const std::shared_ptr<FightVisitor>& attacker) override;
    
    bool fight(const std::shared_ptr<Toad>& other) override;
    bool fight(const std::shared_ptr<Dragon>& other) override;
    bool fight(const std::shared_ptr<Knight>& other) override;
    
    std::string getType() const override { return "Dragon"; }
};