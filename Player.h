#pragma once

#include "Utility/Timer.h"

#include "World.h"
#include "Moveable.h"

class Player : public Moveable {
    float jumpHeight_;
    const float maxJumpHeight_;

    bool jumping_;

   public:
    explicit Player(SoundManager& soundManager);
    Player(const sf::Vector2f& position, const sf::Vector2f& size, sf::Color playerColor, SoundManager& soundManager);

    void update(float elapsedTime, sf::RenderWindow& window, World& world) override;

    virtual void onEnemyKill();

    MoveableType getType() override;

    std::string toString() override;
};
