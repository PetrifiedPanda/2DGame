cmake_minimum_required(VERSION 3.10.2)

set(This 2DGame)

project(${This})

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(Sources
    Main.cpp
    Global.cpp
    World.cpp
    View.cpp
    SoundManager.cpp
    Moveable.cpp
    Enemy.cpp
    Player.cpp
    GrowingPlayer.cpp
    Utility/Timer.cpp
    Utility/AverageCollector.cpp
)

add_executable(${This} ${Sources})

target_link_libraries(${This} sfml-graphics sfml-audio sfml-window sfml-system sfml-audio gomp)