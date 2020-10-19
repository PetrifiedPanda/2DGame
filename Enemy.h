#pragma once

#include "Utility/Timer.h"

#include "World.h"
#include "Player.h"

class Enemy : public Moveable {
    sf::RectangleShape hitBox_;

    bool movingLeft_;

   public:
    explicit Enemy(SoundManager& soundManager);
    Enemy(const Vector2f& position, float playerSizeX, float playerSizeY, sf::Color enemyColor, SoundManager& soundManager);

    void update(float elapsedTime, sf::RenderWindow& window, World& world) override;

    MoveableType getType() override;

    std::string toString() override;

    void setPosition(const Vector2f& position) override;

   protected:
    void move(const Vector2f& direction) override;
    void setSize(const Vector2f& size) override;
    void repositionHitBox();

    bool isAttacked(Moveable* player) const;
};
