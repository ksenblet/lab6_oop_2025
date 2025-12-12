#include <iostream>

#include "dragon.h"
#include "fightVisitor.h"
#include "toad.h"   
#include "knight.h"
#include "observer.h"

Dragon::Dragon(const std::string& name, int x, int y) : NPC(name, x, y) {}

bool Dragon::accept(const std::shared_ptr<FightVisitor>& attacker) {
    return attacker->visit(std::dynamic_pointer_cast<Dragon>(shared_from_this()));
}

bool Dragon::fight(const std::shared_ptr<Toad>& other) {
    std::cout << "Dragon " << name << " fights Toad " << other->getName() << " - Toad wins (eats all)\n";
    return false;  // Дракон проигрывает жабе
}

bool Dragon::fight(const std::shared_ptr<Dragon>& other) {
    std::cout << "Dragon " << name << " fights Dragon " << other->getName() << " - Draw\n";
    return false;  // Ничья
}

bool Dragon::fight(const std::shared_ptr<Knight>& other) {
    std::cout << "Dragon " << name << " fights Knight " << other->getName() << " - Dragon wins\n";
    return true;  // Дракон побеждает рыцаря
}