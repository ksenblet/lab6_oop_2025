#include <gtest/gtest.h>
#include <sstream>
#include <memory>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

#include "factory.h"
#include "toad.h"
#include "dragon.h"
#include "knight.h"

class FactoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаем тестовые потоки
        toad_stream = std::make_unique<std::istringstream>("Toad Toad1 10 20");
        dragon_stream = std::make_unique<std::istringstream>("Dragon Dragon1 30 40");
        knight_stream = std::make_unique<std::istringstream>("Knight Knight1 50 60");
        
        // Инициализируем генератор случайных чисел
        std::srand(static_cast<unsigned>(std::time(nullptr)));
    }
    
    void TearDown() override {
        // Удаляем тестовые файлы если они существуют
        std::remove("temp_test_npcs.txt");
        std::remove("temp_mixed_test.txt");
        // Удаляем файлы с уникальными именами
        for (const auto& filename : temp_files_to_remove) {
            std::remove(filename.c_str());
        }
    }
    
    std::string createUniqueFilename(const std::string& prefix) {
        std::string filename = prefix + "_" + std::to_string(std::rand()) + ".txt";
        temp_files_to_remove.push_back(filename);
        return filename;
    }
    
    std::unique_ptr<std::istringstream> toad_stream;
    std::unique_ptr<std::istringstream> dragon_stream;
    std::unique_ptr<std::istringstream> knight_stream;
    std::vector<std::string> temp_files_to_remove;
};


TEST_F(FactoryTest, CreateByType) {
    auto toad = NPCFactory::create(NpcType::Toad, "FactoryToad", 1, 2);
    auto dragon = NPCFactory::create(NpcType::Dragon, "FactoryDragon", 3, 4);
    auto knight = NPCFactory::create(NpcType::Knight, "FactoryKnight", 5, 6);
    
    ASSERT_NE(toad, nullptr);
    ASSERT_NE(dragon, nullptr);
    ASSERT_NE(knight, nullptr);
    
    EXPECT_EQ(toad->getType(), "Toad");
    EXPECT_EQ(dragon->getType(), "Dragon");
    EXPECT_EQ(knight->getType(), "Knight");
    
    EXPECT_EQ(toad->getName(), "FactoryToad");
    EXPECT_EQ(toad->getX(), 1);
    EXPECT_EQ(toad->getY(), 2);
}

TEST_F(FactoryTest, CreateFromStream) {
    auto toad = NPCFactory::create(*toad_stream);
    auto dragon = NPCFactory::create(*dragon_stream);
    auto knight = NPCFactory::create(*knight_stream);
    
    ASSERT_NE(toad, nullptr);
    ASSERT_NE(dragon, nullptr);
    ASSERT_NE(knight, nullptr);
    
    EXPECT_EQ(toad->getName(), "Toad1");
    EXPECT_EQ(toad->getX(), 10);
    EXPECT_EQ(toad->getY(), 20);
    
    EXPECT_EQ(dragon->getName(), "Dragon1");
    EXPECT_EQ(knight->getName(), "Knight1");
}

TEST_F(FactoryTest, SaveToStream) {
    auto toad = std::make_shared<Toad>("SavedToad", 100, 200);
    auto dragon = std::make_shared<Dragon>("SavedDragon", 300, 400);
    
    std::ostringstream output;
    NPCFactory::save(toad, output);
    NPCFactory::save(dragon, output);
    
    std::string result = output.str();
    EXPECT_NE(result.find("Toad SavedToad 100 200"), std::string::npos);
    EXPECT_NE(result.find("Dragon SavedDragon 300 400"), std::string::npos);
}

TEST_F(FactoryTest, InvalidType) {
    auto invalid = NPCFactory::create(static_cast<NpcType>(999), "Invalid", 0, 0);
    EXPECT_EQ(invalid, nullptr);
}

TEST_F(FactoryTest, InvalidStreamData) {
    std::istringstream bad_stream1("InvalidType Name 10 20");
    std::istringstream bad_stream2("Toad OnlyName");
    std::istringstream bad_stream3("");
    
    auto npc1 = NPCFactory::create(bad_stream1);
    auto npc2 = NPCFactory::create(bad_stream2);
    auto npc3 = NPCFactory::create(bad_stream3);
    
    EXPECT_EQ(npc1, nullptr);
    EXPECT_EQ(npc2, nullptr);
    EXPECT_EQ(npc3, nullptr);
}

TEST_F(FactoryTest, RoundTripSaveLoad) {
    // Создаем NPC
    auto original = std::make_shared<Toad>("RoundTrip", 123, 456);
    
    // Сохраняем в поток
    std::ostringstream save_stream;
    NPCFactory::save(original, save_stream);
    
    // Загружаем из потока
    std::istringstream load_stream(save_stream.str());
    auto loaded = NPCFactory::create(load_stream);
    
    ASSERT_NE(loaded, nullptr);
    EXPECT_EQ(loaded->getType(), "Toad");
    EXPECT_EQ(loaded->getName(), "RoundTrip");
    EXPECT_EQ(loaded->getX(), 123);
    EXPECT_EQ(loaded->getY(), 456);
}

TEST_F(FactoryTest, LoadFromFileWithMultipleNPCs) {
    // Создаем уникальное имя файла в текущей директории
    std::string test_filename = createUniqueFilename("test_npcs");
    
    // Создаем файл для теста
    std::ofstream create_file(test_filename);
    ASSERT_TRUE(create_file.is_open()) << "Cannot create file: " << test_filename;
    
    // ВСЕ координаты должны быть в диапазоне 0-500
    create_file << "Toad Toad_Ivan 100 200\n";           // OK: 100, 200
    create_file << "Dragon Dragon_Petr 300 400\n";       // OK: 300, 400
    create_file << "Knight Knight_Vasya 500 100\n";      // OK: 500, 100 (граница)
    create_file << "Toad Toad_Masha 150 250\n";          // OK: 150, 250
    create_file << "Dragon Dragon_Olya 350 450\n";       // OK: 350, 450
    create_file << "Knight Knight_Kolya 400 300\n";      // OK: 400, 300 (было 600, 300)
    create_file.close();
    
    // Открываем файл и загружаем NPC
    std::ifstream input_file(test_filename);
    ASSERT_TRUE(input_file.is_open()) << "Cannot open file: " << test_filename;
    
    std::vector<std::shared_ptr<NPC>> loaded_npcs;
    std::string line;
    int line_number = 0;
    
    while (std::getline(input_file, line)) {
        line_number++;
        std::istringstream line_stream(line);
        auto npc = NPCFactory::create(line_stream);
        
        if (npc) {
            loaded_npcs.push_back(npc);
        } else {
            // Для отладки, если какая-то строка не загрузилась
            std::cout << "Failed to load NPC from line " << line_number 
                      << ": " << line << std::endl;
        }
    }
    
    input_file.close();
    
    // Проверяем, что все NPC загрузились
    ASSERT_EQ(loaded_npcs.size(), 6) << "Expected 6 NPC, loaded " << loaded_npcs.size();
    
    // Проверяем типы и данные NPC
    // NPC 1: Toad Ivan
    EXPECT_EQ(loaded_npcs[0]->getType(), "Toad");
    EXPECT_EQ(loaded_npcs[0]->getName(), "Toad_Ivan");
    EXPECT_EQ(loaded_npcs[0]->getX(), 100);
    EXPECT_EQ(loaded_npcs[0]->getY(), 200);
    
    // NPC 2: Dragon Petr
    EXPECT_EQ(loaded_npcs[1]->getType(), "Dragon");
    EXPECT_EQ(loaded_npcs[1]->getName(), "Dragon_Petr");
    EXPECT_EQ(loaded_npcs[1]->getX(), 300);
    EXPECT_EQ(loaded_npcs[1]->getY(), 400);
    
    // NPC 3: Knight Vasya
    EXPECT_EQ(loaded_npcs[2]->getType(), "Knight");
    EXPECT_EQ(loaded_npcs[2]->getName(), "Knight_Vasya");
    EXPECT_EQ(loaded_npcs[2]->getX(), 500);
    EXPECT_EQ(loaded_npcs[2]->getY(), 100);
    
    // NPC 4: Toad Masha
    EXPECT_EQ(loaded_npcs[3]->getType(), "Toad");
    EXPECT_EQ(loaded_npcs[3]->getName(), "Toad_Masha");
    EXPECT_EQ(loaded_npcs[3]->getX(), 150);
    EXPECT_EQ(loaded_npcs[3]->getY(), 250);
    
    // NPC 5: Dragon Olya
    EXPECT_EQ(loaded_npcs[4]->getType(), "Dragon");
    EXPECT_EQ(loaded_npcs[4]->getName(), "Dragon_Olya");
    EXPECT_EQ(loaded_npcs[4]->getX(), 350);
    EXPECT_EQ(loaded_npcs[4]->getY(), 450);
    
    // NPC 6: Knight Kolya (исправленные координаты)
    EXPECT_EQ(loaded_npcs[5]->getType(), "Knight");
    EXPECT_EQ(loaded_npcs[5]->getName(), "Knight_Kolya");
    EXPECT_EQ(loaded_npcs[5]->getX(), 400);    // Было 600, стало 400
    EXPECT_EQ(loaded_npcs[5]->getY(), 300);
}

TEST_F(FactoryTest, SaveMultipleNPCsToFileAndReload) {
    // Создаем тестовые NPC
    std::vector<std::shared_ptr<NPC>> original_npcs = {
        std::make_shared<Toad>("TestToad1", 11, 22),
        std::make_shared<Dragon>("TestDragon1", 33, 44),
        std::make_shared<Knight>("TestKnight1", 55, 66),
        std::make_shared<Toad>("TestToad2", 77, 88),
        std::make_shared<Dragon>("TestDragon2", 99, 111)
    };
    
    // Создаем уникальное имя файла
    std::string temp_filename = createUniqueFilename("temp_npcs");
    
    // Сохраняем в файл
    std::ofstream out_file(temp_filename);
    ASSERT_TRUE(out_file.is_open()) << "Failed to create file: " << temp_filename;
    
    for (const auto& npc : original_npcs) {
        NPCFactory::save(npc, out_file);
    }
    out_file.close();
    
    // Загружаем обратно
    std::ifstream in_file(temp_filename);
    ASSERT_TRUE(in_file.is_open()) << "Failed to open file for reading: " << temp_filename;
    
    std::vector<std::shared_ptr<NPC>> loaded_npcs;
    std::string line;
    
    while (std::getline(in_file, line)) {
        std::istringstream line_stream(line);
        auto npc = NPCFactory::create(line_stream);
        if (npc) {
            loaded_npcs.push_back(npc);
        }
    }
    in_file.close();
    
    // Проверяем что загрузилось столько же NPC
    ASSERT_EQ(original_npcs.size(), loaded_npcs.size());
    
    // Проверяем что данные совпадают
    for (size_t i = 0; i < original_npcs.size(); ++i) {
        EXPECT_EQ(original_npcs[i]->getType(), loaded_npcs[i]->getType());
        EXPECT_EQ(original_npcs[i]->getName(), loaded_npcs[i]->getName());
        EXPECT_EQ(original_npcs[i]->getX(), loaded_npcs[i]->getX());
        EXPECT_EQ(original_npcs[i]->getY(), loaded_npcs[i]->getY());
    }
}

TEST_F(FactoryTest, MixedValidAndInvalidFileContent) {
    // Создаем тестовый файл со смесью правильных и неправильных строк
    std::string temp_filename = createUniqueFilename("mixed_test");
    
    std::ofstream mixed_file(temp_filename);
    ASSERT_TRUE(mixed_file.is_open()) << "Failed to create file: " << temp_filename;
    
    mixed_file << "Toad ValidToad1 10 20\n";        // Valid
    mixed_file << "InvalidType WrongNPC 30 40\n";   // Invalid
    mixed_file << "Dragon ValidDragon1 50 60\n";    // Valid
    mixed_file << "Knight OnlyName\n";              // Invalid (missing coords)
    mixed_file << "Toad ValidToad2 70 80\n";        // Valid
    mixed_file << "Wrong Wrong Wrong Wrong\n";      // Invalid
    mixed_file << "Knight ValidKnight1 90 100\n";   // Valid
    mixed_file.close();
    
    // Загружаем
    std::ifstream in_file(temp_filename);
    ASSERT_TRUE(in_file.is_open()) << "Failed to open file: " << temp_filename;
    
    std::vector<std::shared_ptr<NPC>> loaded_npcs;
    std::string line;
    int valid_lines = 0, invalid_lines = 0;
    
    while (std::getline(in_file, line)) {
        std::istringstream line_stream(line);
        auto npc = NPCFactory::create(line_stream);
        if (npc) {
            loaded_npcs.push_back(npc);
            valid_lines++;
        } else {
            invalid_lines++;
        }
    }
    in_file.close();
    
    // В файле было 7 строк, из них должно быть 4 валидных
    EXPECT_EQ(valid_lines, 4);
    EXPECT_EQ(invalid_lines, 3);
    EXPECT_EQ(loaded_npcs.size(), 4);
    
    // Проверяем валидные NPC
    if (loaded_npcs.size() >= 4) {
        EXPECT_EQ(loaded_npcs[0]->getType(), "Toad");
        EXPECT_EQ(loaded_npcs[0]->getName(), "ValidToad1");
        
        EXPECT_EQ(loaded_npcs[1]->getType(), "Dragon");
        EXPECT_EQ(loaded_npcs[1]->getName(), "ValidDragon1");
        
        EXPECT_EQ(loaded_npcs[2]->getType(), "Toad");
        EXPECT_EQ(loaded_npcs[2]->getName(), "ValidToad2");
        
        EXPECT_EQ(loaded_npcs[3]->getType(), "Knight");
        EXPECT_EQ(loaded_npcs[3]->getName(), "ValidKnight1");
    }
}