cmake_minimum_required(VERSION 3.15)
project(dungeon_editor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_SOURCE_DIR}/include)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
endif()

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.0
    TLS_VERIFY false
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(dungeon_editor
    src/main.cpp
    src/npc.cpp
    src/dragon.cpp
    src/knight.cpp
    src/toad.cpp
    src/fightVisitor.cpp
    src/observer.cpp
    src/factory.cpp
)

add_executable(dungeon_tests
    tests/test_main.cpp
    tests/test_npc.cpp
    tests/test_factory.cpp
    tests/test_fightVisitor.cpp
    tests/test_observer.cpp
    src/npc.cpp
    src/dragon.cpp
    src/knight.cpp
    src/toad.cpp
    src/fightVisitor.cpp
    src/observer.cpp
    src/factory.cpp
)

target_include_directories(dungeon_editor PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_include_directories(dungeon_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(dungeon_editor)
target_link_libraries(dungeon_tests gtest gtest_main)

enable_testing()
add_test(NAME dungeon_tests COMMAND dungeon_tests)

target_compile_features(dungeon_editor PRIVATE cxx_std_20)
target_compile_features(dungeon_tests PRIVATE cxx_std_20)