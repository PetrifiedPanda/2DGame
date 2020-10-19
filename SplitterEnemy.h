#pragma once

#include "Enemy.h"

class SplitterEnemy : public Enemy {
    static const int s_numChildSpawns = 2;
    const int generationNum_;

   public:
    SplitterEnemy(int generationNum = 0);
    SplitterEnemy(const sf::Vector2f& position, const sf::Vector2f& size, sf::Color enemyColor, int generationNum = 0);

    void onDeath(World& world) override;
};