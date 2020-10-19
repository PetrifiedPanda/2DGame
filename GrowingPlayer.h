#pragma once

#include "Utility/Timer.h"

#include "Player.h"

class GrowingPlayer : public Player {
    bool isGrowing_;
    sf::Vector2f growthTarget_;

   public:
    GrowingPlayer();
    GrowingPlayer(const sf::Vector2f& position, const sf::Vector2f& size, sf::Color playerColor);

    void update(float elapsedTime, sf::RenderWindow& window, World& world) override;

    void onEnemyKill() override;

    MoveableType getType() override;

    std::string toString() override;
};
