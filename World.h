#pragma once

#include <omp.h>
#include <array>

#include "SFML/Graphics.hpp"

#include "Utility/Timer.h"

#include "Global.h"
#include "SoundManager.h"

class Moveable;

class World {
    std::vector<sf::RectangleShape> staticRectangles_;
    std::vector<std::unique_ptr<Moveable>> moveables_;

    std::vector<Moveable*> killNextFrame_;

    int idCounter_;

    Moveable* player_;

    float movementSpeed_;
    float gravity_;

    static const short rectangleLimit_ = 500;
    static const short moveableLimit_ = 128;

   public:
    SoundManager soundManager = SoundManager("resources/sound/BackgroundMusic.wav", "resources/sound/Collision.wav", "resources/sound/Death.wav", "resources/sound/Pause.wav");
    World(float movementSpeed = 100.0f, float gravity = 0.0f);

    void addMoveable(std::unique_ptr<Moveable>&& moveable);
    void addRectangle(const sf::RectangleShape& rectangle);
    void addPlayer(std::unique_ptr<Moveable>&& player);

    void killMoveable(Moveable* moveable);

    int getMoveableCount() const;
    int getStaticRectangleCount() const;

    float getGravity() const;
    float getMovementSpeed() const;

    sf::Vector2f getMaxMovement(Moveable* moveable, const sf::Vector2f& direction);
    sf::Vector2f getDistanceUntilCollision(const sf::RectangleShape& rect1, const sf::RectangleShape& rect2, const sf::Vector2f& direction);

    bool canMoveInDirection(Moveable* moveable, const sf::Vector2f& direction);
    static bool doRectanglesOverlap(const sf::RectangleShape& rect, const sf::Vector2f& size, const std::array<sf::Vector2f, 4>& corners);
    static bool doRectanglesOverlap(const sf::RectangleShape& rect, const sf::RectangleShape& otherRect);

    void update(float elapsedTime, sf::RenderWindow& window, Moveable* draggedMoveable);

    void draw(sf::RenderWindow& window);

    Moveable* getPlayer() const;

    Moveable* isOnMoveable(const sf::Vector2f& position);
    bool containsMoveable(Moveable* moveable);

   private:
    void deleteMoveable(Moveable* moveable);

    static bool cornersInsideRectangle(const sf::RectangleShape& rect, const std::array<sf::Vector2f, 4>& corners);
    static std::array<sf::Vector2f, 4> findCorners(const sf::Vector2f& position, const sf::Vector2f& size);
};

#include "Moveable.h"
