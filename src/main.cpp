#include <iostream>
#include <fstream>
#include <memory>
#include <set>
#include <random>
#include <string>
#include <sstream>

#include "npc.h"
#include "factory.h"
#include "observer.h"
#include "fightVisitor.h"

using set_t = std::set<std::shared_ptr<NPC>>;

std::shared_ptr<NPC> createFromStream(std::istream &is)
{
    return NPCFactory::create(is);
}

std::shared_ptr<NPC> createNPC(NpcType type, const std::string& name, int x, int y)
{
    return NPCFactory::create(type, name, x, y);
}

void saveNPC(const set_t &npc_collection, const std::string &file_name)
{
    std::ofstream file(file_name);
    for (auto &n : npc_collection)
        NPCFactory::save(n, file);
    file.flush();
    file.close();
    std::cout << "Saved " << npc_collection.size() << " NPC in " << file_name << std::endl;
}

set_t loadNPC(const std::string &file_name)
{
    set_t loaded;
    std::ifstream file(file_name);
    if (file.good() && file.is_open())
    {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream stream(line);
            auto npc = createFromStream(stream);
            if (npc) {
                loaded.insert(npc);
            }
        }
        file.close();
        std::cout << "Loaded " << loaded.size() << " NPC from " << file_name << std::endl;
    }
    else {
        std::cerr << "Err: can't open file: " << file_name << std::endl;
    }
    return loaded;
}

std::ostream &operator<<(std::ostream &os, const set_t &npc_collection)
{
    os << "Total NPCs: " << npc_collection.size() << std::endl;
    for (auto &n : npc_collection) {
        os << n->getType() << " \"" << n->getName() 
           << "\" at position: (" << n->getX() << ", " << n->getY() << ")" 
           << " - " << (n->isAlive() ? "Alive" : "Dead") << std::endl;
    }
    return os;
}

set_t fight(const set_t &npc_collection, size_t range, const std::shared_ptr<IFFightObserver>& observer = nullptr)
{
    set_t killed_npcs;

    for (const auto &attacker : npc_collection) {
        if (!attacker->isAlive()) continue;
        
        for (const auto &defender : npc_collection) {
            if (!defender->isAlive()) continue;
            if (attacker == defender) continue;

            if (attacker->distance(defender) <= range) {
                auto visitor = std::make_shared<FightVisitor>(attacker, observer);
                bool victory = defender->accept(visitor);
                
                if (victory && defender->isAlive()) {
                    defender->kill();
                    killed_npcs.insert(defender);
                }
            }
        }
    }

    return killed_npcs;
}

std::string generateName(const std::string& type, int n) {
    return type + "_" + std::to_string(n);
}

int main()
{
    set_t game_world;
    auto console_logger = std::make_shared<TextObserver>();
    auto fileLogger = std::make_shared<FileObserver>("fighting_log.txt");

    class CombinedLogger : public IFFightObserver {
    private:
        std::vector<std::shared_ptr<IFFightObserver>> loggers;
    public:
        void addLogger(const std::shared_ptr<IFFightObserver>& logger) {
            loggers.push_back(logger);
        }
        
        void onFight(const std::shared_ptr<NPC>& attacker, 
                    const std::shared_ptr<NPC>& defender, 
                    bool result) override {
            for (auto& logger : loggers) {
                logger->onFight(attacker, defender, result);
            }
        }
    };
    
    auto main_logger = std::make_shared<CombinedLogger>();
    main_logger->addLogger(console_logger);
    main_logger->addLogger(fileLogger);

    std::cout << "Creating NPCs..." << std::endl;
    std::random_device rnd;
    std::mt19937 gen_num(rnd());
    std::uniform_int_distribution<> rnd_type(0, 2);
    std::uniform_int_distribution<> rnd_coord(0, 500);
    
    for (size_t i = 0; i < 30; ++i) {
        NpcType type = static_cast<NpcType>(rnd_type(gen_num));
        std::string typeStr;
        switch (type) {
            case NpcType::Toad: typeStr = "Toad"; break;
            case NpcType::Dragon: typeStr = "Dragon"; break;
            case NpcType::Knight: typeStr = "Knight"; break;
        }
        
        game_world.insert(createNPC(type, generateName(typeStr, i), 
                                rnd_coord(gen_num), rnd_coord(gen_num)));
    }

    std::cout << "Saving..." << std::endl;
    saveNPC(game_world, "save.txt");

    std::cout << "Loading..." << std::endl;
    game_world = loadNPC("save.txt");

    std::cout << "Initial state:" << std::endl << game_world << std::endl;

    std::cout << "Start..." << std::endl;

    for (size_t range = 20; range <= 100 && !game_world.empty(); range += 15)
{
    auto dead = fight(game_world, range, main_logger);
    
    std::cout << "     Battle statistics     " << std::endl
              << "Range: " << range << std::endl
              << "Killed: " << dead.size() << std::endl;
    
    if (!dead.empty()) {
        std::cout << "Killed NPCs:" << std::endl;
        for (const auto& d : dead) {
            std::cout << "  " << d->getType() << " " << d->getName() 
                      << " at (" << d->getX() << ", " << d->getY() << ")" << std::endl;
        }
    }
    
    for (auto &d : dead) {
        game_world.erase(d);
    }
    
    std::cout << "Alive: " << game_world.size() << std::endl
              << std::endl;
}

std::cout << "Final alive:" << std::endl << game_world;

return 0;
}