#pragma once

#include <memory>
#include <iostream>
#include <fstream>

#include "npc.h"

// печать на экран/файл
class IFFightObserver {
public:
    virtual ~IFFightObserver() = default;
    virtual void onFight(const std::shared_ptr<NPC>& attacker, const std::shared_ptr<NPC>& defender, bool success) = 0;
};

class TextObserver : public IFFightObserver {
public:
    void onFight(const std::shared_ptr<NPC>& attacker, const std::shared_ptr<NPC>& defender, bool success) override;
};

class FileObserver : public IFFightObserver {
private:
    std::ofstream logfile;

public:
    FileObserver(const std::string& filename = "logs_of_battle.txt");
    ~FileObserver();
    
    void onFight(const std::shared_ptr<NPC>& attacker, const std::shared_ptr<NPC>& defender, bool success) override;
};