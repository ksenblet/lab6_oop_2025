#include <gtest/gtest.h>
#include <memory>

#include "toad.h"
#include "dragon.h"
#include "knight.h"
#include "fightVisitor.h"
#include "observer.h"

class MockObserver : public IFFightObserver {
public:
    int call_count = 0;
    std::shared_ptr<NPC> last_attacker;
    std::shared_ptr<NPC> last_defender;
    bool last_result = false;
    
    void onFight(const std::shared_ptr<NPC>& attacker, 
                 const std::shared_ptr<NPC>& defender, 
                 bool result) override {
        call_count++;
        last_attacker = attacker;
        last_defender = defender;
        last_result = result;
    }
    
    void reset() {
        call_count = 0;
        last_attacker.reset();
        last_defender.reset();
        last_result = false;
    }
};

class FightVisitorTest : public ::testing::Test {
protected:
    void SetUp() override {
        toad = std::make_shared<Toad>("Toad", 0, 0);
        dragon = std::make_shared<Dragon>("Dragon", 1, 1);
        knight = std::make_shared<Knight>("Knight", 2, 2);
        mock_observer = std::make_shared<MockObserver>();
    }
    
    std::shared_ptr<Toad> toad;
    std::shared_ptr<Dragon> dragon;
    std::shared_ptr<Knight> knight;
    std::shared_ptr<MockObserver> mock_observer;
};

TEST_F(FightVisitorTest, ToadAlwaysWins) {
    // Toad vs Dragon
    auto visitor = std::make_shared<FightVisitor>(toad);
    EXPECT_TRUE(visitor->visit(dragon));
    
    // Toad vs Knight
    EXPECT_TRUE(visitor->visit(knight));
    
    // Toad vs Toad
    auto another_toad = std::make_shared<Toad>("AnotherToad", 3, 3);
    EXPECT_TRUE(visitor->visit(another_toad));
}

TEST_F(FightVisitorTest, DragonWinsVsKnight) {
    auto visitor = std::make_shared<FightVisitor>(dragon);
    EXPECT_TRUE(visitor->visit(knight));
}

TEST_F(FightVisitorTest, DragonLosesVsToad) {
    auto visitor = std::make_shared<FightVisitor>(dragon);
    EXPECT_FALSE(visitor->visit(toad));
}

TEST_F(FightVisitorTest, DragonDrawVsDragon) {
    auto another_dragon = std::make_shared<Dragon>("AnotherDragon", 3, 3);
    auto visitor = std::make_shared<FightVisitor>(dragon);
    EXPECT_FALSE(visitor->visit(another_dragon));
}

TEST_F(FightVisitorTest, KnightWinsVsDragon) {
    auto visitor = std::make_shared<FightVisitor>(knight);
    EXPECT_TRUE(visitor->visit(dragon));
}

TEST_F(FightVisitorTest, KnightLosesVsToad) {
    auto visitor = std::make_shared<FightVisitor>(knight);
    EXPECT_FALSE(visitor->visit(toad));
}

TEST_F(FightVisitorTest, KnightDrawVsKnight) {
    auto another_knight = std::make_shared<Knight>("AnotherKnight", 3, 3);
    auto visitor = std::make_shared<FightVisitor>(knight);
    EXPECT_FALSE(visitor->visit(another_knight));
}

TEST_F(FightVisitorTest, ObserverNotificationOnSuccess) {
    mock_observer->reset();
    auto visitor = std::make_shared<FightVisitor>(toad, mock_observer);
    
    visitor->visit(dragon);
    
    EXPECT_EQ(mock_observer->call_count, 1);
    EXPECT_EQ(mock_observer->last_attacker, toad);
    EXPECT_EQ(mock_observer->last_defender, dragon);
    EXPECT_TRUE(mock_observer->last_result);
}

TEST_F(FightVisitorTest, ObserverNotificationOnDraw) {
    mock_observer->reset();
    auto another_dragon = std::make_shared<Dragon>("AnotherDragon", 3, 3);
    auto visitor = std::make_shared<FightVisitor>(dragon, mock_observer);
    
    visitor->visit(another_dragon);
    
    EXPECT_EQ(mock_observer->call_count, 1);
    EXPECT_EQ(mock_observer->last_attacker, dragon);
    EXPECT_EQ(mock_observer->last_defender, another_dragon);
    EXPECT_FALSE(mock_observer->last_result);
}

TEST_F(FightVisitorTest, GetAttacker) {
    auto visitor = std::make_shared<FightVisitor>(toad);
    EXPECT_EQ(visitor->getAttacker(), toad);
    
    visitor = std::make_shared<FightVisitor>(dragon);
    EXPECT_EQ(visitor->getAttacker(), dragon);
    
    visitor = std::make_shared<FightVisitor>(knight);
    EXPECT_EQ(visitor->getAttacker(), knight);
}

TEST_F(FightVisitorTest, VisitorWithoutObserver) {
    // Должен работать без крашей даже без наблюдателя
    auto visitor = std::make_shared<FightVisitor>(toad, nullptr);
    EXPECT_NO_THROW(visitor->visit(dragon));
    
    visitor = std::make_shared<FightVisitor>(toad);
    EXPECT_NO_THROW(visitor->visit(knight));
}