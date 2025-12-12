#pragma once

#include <memory>
#include <string>
#include <iostream>

#include "npc.h"

enum class NpcType {
    Toad,
    Dragon, 
    Knight
};

// создание + загрузка
class NPCFactory {
public:
    // нпс создан
    static std::shared_ptr<NPC> create(NpcType type, const std::string& name, int x, int y);
    // из файла
    static std::shared_ptr<NPC> create(std::istream& is);
    // в файл
    static void save(const std::shared_ptr<NPC>& npc, std::ostream& os);
};