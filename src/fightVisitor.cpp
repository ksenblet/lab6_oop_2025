#include "fightVisitor.h"
#include "observer.h"
#include "toad.h"    
#include "dragon.h"
#include "knight.h"

FightVisitor::FightVisitor(const std::shared_ptr<NPC>& attacker, const std::shared_ptr<IFFightObserver>& observer)
    : attacker(attacker), observer(observer) {}

bool FightVisitor::visit(const std::shared_ptr<Toad>& defender) {   
    bool success = attacker->fight(defender);
    if (observer) { // наблюдатель передается в визитор
        observer->onFight(attacker, defender, success);
    }
    return success;
}

bool FightVisitor::visit(const std::shared_ptr<Dragon>& defender) {
    bool success = attacker->fight(defender);
    if (observer) {
        observer->onFight(attacker, defender, success);
    }
    return success;
}

bool FightVisitor::visit(const std::shared_ptr<Knight>& defender) {
    bool success = attacker->fight(defender);
    if (observer) {
        observer->onFight(attacker, defender, success);
    }
    return success;
}