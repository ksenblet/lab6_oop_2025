#include <gtest/gtest.h>
#include <sstream>
#include <fstream>
#include <memory>
#include <cstdio>

#include "observer.h"
#include "toad.h"
#include "dragon.h"
#include "knight.h"

class ObserverTest : public ::testing::Test {
protected:
    void SetUp() override {
        attacker = std::make_shared<Toad>("AttackerToad", 10, 20);
        defender = std::make_shared<Dragon>("DefenderDragon", 30, 40);
    }
    
    void TearDown() override {
        // Удаляем тестовые файлы если они существуют
        std::remove("test_log.txt");
        std::remove("test_log_append.txt");
        std::remove("test_log_empty.txt");
    }
    
    std::shared_ptr<NPC> attacker;
    std::shared_ptr<NPC> defender;
};

TEST_F(ObserverTest, TextObserverOutputSuccessfulFight) {
    // Перенаправляем stdout
    std::stringstream buffer;
    std::streambuf* old_cout = std::cout.rdbuf(buffer.rdbuf());
    
    TextObserver observer;
    observer.onFight(attacker, defender, true);
    
    std::cout.rdbuf(old_cout);
    
    std::string output = buffer.str();
    EXPECT_NE(output.find("Toad AttackerToad killed Dragon DefenderDragon"), std::string::npos);
    EXPECT_NE(output.find("at (30, 40)"), std::string::npos);
}

TEST_F(ObserverTest, TextObserverNoOutputFailedFight) {
    std::stringstream buffer;
    std::streambuf* old_cout = std::cout.rdbuf(buffer.rdbuf());
    
    TextObserver observer;
    observer.onFight(attacker, defender, false);
    
    std::cout.rdbuf(old_cout);
    
    EXPECT_TRUE(buffer.str().empty());
}

TEST_F(ObserverTest, FileObserverCreatesAndWritesFile) {
    const std::string filename = "test_log.txt";
    
    {
        FileObserver observer(filename);
        observer.onFight(attacker, defender, true);
    }
    
    std::ifstream file(filename);
    ASSERT_TRUE(file.is_open());
    
    std::string line;
    std::getline(file, line);
    
    EXPECT_NE(line.find("Toad AttackerToad killed Dragon DefenderDragon"), std::string::npos);
    EXPECT_NE(line.find("at (30, 40)"), std::string::npos);
    
    file.close();
}

TEST_F(ObserverTest, FileObserverAppendsToExistingFile) {
    const std::string filename = "test_log_append.txt";
    
    // Первая запись
    {
        FileObserver observer1(filename);
        observer1.onFight(attacker, defender, true);
    }
    
    // Вторая запись
    auto knight = std::make_shared<Knight>("KnightAttacker", 50, 60);
    {
        FileObserver observer2(filename);
        observer2.onFight(knight, attacker, true);
    }
    
    std::ifstream file(filename);
    std::vector<std::string> lines;
    std::string line;
    
    while (std::getline(file, line)) {
        lines.push_back(line);
    }
    
    EXPECT_EQ(lines.size(), 2); // Только 2 успешных боя
    EXPECT_NE(lines[0].find("Toad"), std::string::npos);
    EXPECT_NE(lines[1].find("Knight"), std::string::npos);
    
    file.close();
    std::remove(filename.c_str());
}