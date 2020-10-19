#pragma once

#include "World.h"
#include "SoundManager.h"

enum class MoveableType {
    PLAYER,
    GROWINGPLAYER,
    ENEMY
};

class Moveable {
    static const int idNullValue = -1;
    int id_;

   protected:
    sf::RectangleShape rectangle_;

    Vector2f force_;

    SoundManager& soundManager_;

   public:
    explicit Moveable(SoundManager& soundManager);
    explicit Moveable(const Vector2f& position, SoundManager& soundManager);
    virtual ~Moveable();

    virtual void update(float elapsedTime, sf::RenderWindow& window, World& world);

    void addForce(const Vector2f& force);

    virtual void die(World& world);

    sf::RectangleShape getRectangle() const;
    Vector2f getSize() const;
    Vector2f getPosition() const;

    virtual void setPosition(const Vector2f& position);
    void setTexture(sf::Texture* texture);

    virtual MoveableType getType() = 0;
    std::string getTypeString();
    int getID() const;
    void setID(int newID);

    virtual std::string toString();

   protected:
    virtual void move(const Vector2f& direction);
    virtual void setSize(const Vector2f& size);

    void setFillColor(const sf::Color& color);
    void setOutlineColor(const sf::Color& color);
    void setOutlineThickness(float thickness);
};