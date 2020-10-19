#pragma once

#include "Enemy.h"

class SplitterEnemy : public Enemy {
   public:
    SplitterEnemy();
    SplitterEnemy(const sf::Vector2f& position, const sf::Vector2f& size, sf::Color enemyColor);

    void onDeath(World& world) override;
};