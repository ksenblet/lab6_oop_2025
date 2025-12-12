#include "observer.h"

void TextObserver::onFight(const std::shared_ptr<NPC>& attacker,const std::shared_ptr<NPC>& defender,bool success) {
    if (success) {
        std::cout << attacker->getType() << " " << attacker->getName() << " killed " << defender->getType() << " " << defender->getName() << " at (" << defender->getX() << ", " << defender->getY() << ")\n";
    }
}

FileObserver::FileObserver(const std::string& filename) {
    logfile.open(filename, std::ios::app);
}

FileObserver::~FileObserver() {
    if (logfile.is_open()) {
        logfile.close();
    }
}

void FileObserver::onFight(const std::shared_ptr<NPC>& attacker,const std::shared_ptr<NPC>& defender, bool success) {
    if (logfile.is_open() && success) {
        logfile << attacker->getType() << " " << attacker->getName() 
                << " killed " << defender->getType() << " " << defender->getName() 
                << " at (" << defender->getX() << ", " << defender->getY() << ")\n";
    }
}