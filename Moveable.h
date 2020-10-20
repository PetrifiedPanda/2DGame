#pragma once

#include "World.h"
#include "SoundManager.h"

enum class MoveableType {
    PLAYER,
    GROWINGPLAYER,
    ENEMY,
    SPLITTERENEMY
};

class Moveable {
    static const int s_idNullValue = -1;
    int id_;

   protected:
    sf::RectangleShape rectangle_;

    sf::Vector2f force_;

   public:
    explicit Moveable();
    explicit Moveable(const sf::Vector2f& position, const sf::Vector2f& size);
    virtual ~Moveable();

    virtual void update(float elapsedTime, sf::RenderWindow& window, World& world) = 0;
    void gravityUpdate(float elapsedTime, sf::RenderWindow& window, World& world);

    void addForce(const sf::Vector2f& force);

    void die(World& world);
    virtual void onDeath(World& worlds);

    const sf::RectangleShape& getRectangle() const;
    sf::Vector2f getSize() const;
    sf::Vector2f getPosition() const;

    sf::Color getFillColor() const;

    virtual void setPosition(const sf::Vector2f& position);
    void setTexture(sf::Texture* texture);

    virtual MoveableType getType() const = 0;
    std::string getTypeString() const;
    int getID() const;
    void setID(int newID);

    virtual std::string toString() const;

   protected:
    virtual void move(const sf::Vector2f& direction);
    virtual void setSize(const sf::Vector2f& size);

    void setFillColor(const sf::Color& color);
    void setOutlineColor(const sf::Color& color);
    void setOutlineThickness(float thickness);
};