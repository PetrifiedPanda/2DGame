#pragma once

#include "Utility/Timer.h"

#include "World.h"
#include "Moveable.h"

class Player : public Moveable {
    float jumpHeight_;
    const float maxJumpHeight_;

    bool jumping_;

   public:
    Player();
    Player(const sf::Vector2f& position, const sf::Vector2f& size, sf::Color playerColor);

    void update(float elapsedTime, sf::RenderWindow& window, World& world) override;

    virtual void onEnemyKill();

    MoveableType getType() override;

    std::string toString() override;
};
