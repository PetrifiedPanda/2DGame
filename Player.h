#pragma once

#include "Utility/Timer.h"

#include "World.h"
#include "Moveable.h"

class Player : public Moveable {
    float jumpHeight_;
    float maxJumpHeight_;

    bool jumping_;
    bool movingUp_;
    bool movingDown_;
    bool movingLeft_;
    bool movingRight_;

   public:
    explicit Player(SoundManager& soundManager);
    Player(const Vector2f& position, float sizeX, float sizeY, sf::Color playerColor, SoundManager& soundManager);

    void update(float elapsedTime, sf::RenderWindow& window, World& world) override;

    virtual void onEnemyKill();

    MoveableType getType() override;

    std::string toString() override;
};
