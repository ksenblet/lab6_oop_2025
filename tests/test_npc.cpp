#include <gtest/gtest.h>
#include <memory>

#include "npc.h"
#include "toad.h"
#include "dragon.h"
#include "knight.h"

class NPCTest : public ::testing::Test {
protected:
    void SetUp() override {
        toad = std::make_shared<Toad>("TestToad", 10, 20);
        dragon = std::make_shared<Dragon>("TestDragon", 30, 40);
        knight = std::make_shared<Knight>("TestKnight", 50, 60);
    }
    
    std::shared_ptr<Toad> toad;
    std::shared_ptr<Dragon> dragon;
    std::shared_ptr<Knight> knight;
};

TEST_F(NPCTest, BasicProperties) {
    EXPECT_EQ(toad->getName(), "TestToad");
    EXPECT_EQ(toad->getX(), 10);
    EXPECT_EQ(toad->getY(), 20);
    EXPECT_TRUE(toad->isAlive());
    
    EXPECT_EQ(dragon->getName(), "TestDragon");
    EXPECT_EQ(knight->getName(), "TestKnight");
}

TEST_F(NPCTest, TypeNames) {
    EXPECT_EQ(toad->getType(), "Toad");
    EXPECT_EQ(dragon->getType(), "Dragon");
    EXPECT_EQ(knight->getType(), "Knight");
}

TEST_F(NPCTest, DistanceCalculation) {
    auto npc1 = std::make_shared<Toad>("T1", 0, 0);
    auto npc2 = std::make_shared<Toad>("T2", 3, 4);
    
    EXPECT_DOUBLE_EQ(npc1->distance(npc2), 5.0);
    EXPECT_DOUBLE_EQ(npc2->distance(npc1), 5.0);
}

TEST_F(NPCTest, KillMethod) {
    EXPECT_TRUE(toad->isAlive());
    toad->kill();
    EXPECT_FALSE(toad->isAlive());
}

TEST_F(NPCTest, InvalidCoordinates) {
    EXPECT_THROW(std::make_shared<Toad>("BadToad", -1, 0), std::runtime_error);
    EXPECT_THROW(std::make_shared<Toad>("BadToad", 501, 0), std::runtime_error);
    EXPECT_THROW(std::make_shared<Toad>("BadToad", 0, -1), std::runtime_error);
    EXPECT_THROW(std::make_shared<Toad>("BadToad", 0, 501), std::runtime_error);
}

TEST_F(NPCTest, ValidCoordinates) {
    EXPECT_NO_THROW(std::make_shared<Toad>("GoodToad", 0, 0));
    EXPECT_NO_THROW(std::make_shared<Toad>("GoodToad", 500, 500));
    EXPECT_NO_THROW(std::make_shared<Toad>("GoodToad", 250, 250));
}