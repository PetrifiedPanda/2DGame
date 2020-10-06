#pragma once

#include "Utility/AverageCollector.h"
#include "SFML/Graphics.hpp"

extern float g_masterVolume;
extern float g_musicVolume;
extern float g_effectVolume;
extern AverageCollector g_averageCollector;

inline constexpr sf::Keyboard::Key g_upKey = sf::Keyboard::W;
inline constexpr sf::Keyboard::Key g_downKey = sf::Keyboard::S;
inline constexpr sf::Keyboard::Key g_leftKey = sf::Keyboard::A;
inline constexpr sf::Keyboard::Key g_rightKey = sf::Keyboard::D;

inline constexpr sf::Keyboard::Key g_jumpKey = sf::Keyboard::Space;