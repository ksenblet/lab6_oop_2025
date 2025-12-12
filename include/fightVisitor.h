#pragma once

#include <memory>

#include "npc.h"
#include "observer.h"

class Toad;
class Dragon;
class Knight;

// бой
class FightVisitor {
private:
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<IFFightObserver> observer;

public:
    FightVisitor(const std::shared_ptr<NPC>& attacker, const std::shared_ptr<IFFightObserver>& observer = nullptr);
    
    // перегружены для каждого типа нпс
    bool visit(const std::shared_ptr<Toad>& defender);
    bool visit(const std::shared_ptr<Dragon>& defender);
    bool visit(const std::shared_ptr<Knight>& defender);

    std::shared_ptr<NPC> getAttacker() const { return attacker; }
};