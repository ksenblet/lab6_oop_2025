#include <iostream>

#include "knight.h"
#include "fightVisitor.h"
#include "toad.h"   
#include "dragon.h"
#include "observer.h"

Knight::Knight(const std::string& name, int x, int y) : NPC(name, x, y) {}

bool Knight::accept(const std::shared_ptr<FightVisitor>& attacker) {
    return attacker->visit(std::dynamic_pointer_cast<Knight>(shared_from_this()));
}

bool Knight::fight(const std::shared_ptr<Toad>& other) {
    std::cout << "Knight " << name << " fights Toad " << other->getName() << " - Toad wins (eats all)\n";
    return false;  // Рыцарь проигрывает жабе
}

bool Knight::fight(const std::shared_ptr<Dragon>& other) {
    std::cout << "Knight " << name << " fights Dragon " << other->getName() << " - Knight wins\n";
    return true;  // Рыцарь побеждает дракона
}

bool Knight::fight(const std::shared_ptr<Knight>& other) {
    std::cout << "Knight " << name << " fights Knight " << other->getName() << " - Draw\n";
    return false;  // Ничья
}