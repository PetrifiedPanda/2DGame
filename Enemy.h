#pragma once

#include "Utility/Timer.h"

#include "World.h"
#include "Player.h"

class Enemy : public Moveable {
    sf::RectangleShape hitBox_;

    bool movingLeft_;

   public:
    Enemy();
    Enemy(const sf::Vector2f& position, const sf::Vector2f& size, sf::Color enemyColor);

    void update(float elapsedTime, sf::RenderWindow& window, World& world) override;

    MoveableType getType() const override;

    std::string toString() const override;

    void setPosition(const sf::Vector2f& position) override;

   protected:
    void move(const sf::Vector2f& direction) override;
    void setSize(const sf::Vector2f& size) override;
    void repositionHitBox();

    bool isAttacked(Moveable* player) const;
};
