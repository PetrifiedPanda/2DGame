#pragma once

#include <omp.h>
#include <array>

#include "SFML/Graphics.hpp"

#include "Utility/Timer.h"

#include "Global.h"

using sf::Vector2f;

class Moveable;

class World {
   public:
    World();
    World(float movementSpeed, float gravity);

    void addMoveable(std::unique_ptr<Moveable>&& moveable);
    void addRectangle(const sf::RectangleShape& rectangle);
    void addPlayer(std::unique_ptr<Moveable>&& player);

    void killMoveable(Moveable* moveable);

    int getMoveableCount() const;
    int getStaticRectangleCount() const;

    float getGravity() const;
    float getMovementSpeed() const;

    Vector2f getMaxMovement(Moveable* moveable, const Vector2f& direction);
    Vector2f getDistanceUntilCollision(const sf::RectangleShape& rect1, const sf::RectangleShape& rect2, const Vector2f& direction);

    bool canMoveInDirection(Moveable* moveable, const Vector2f& direction);
    static bool doRectanglesOverlap(const sf::RectangleShape& rect, const Vector2f& size, const std::array<Vector2f, 4>& corners);
    static bool doRectanglesOverlap(const sf::RectangleShape& rect, const sf::RectangleShape& otherRect);

    void update(float elapsedTime, sf::RenderWindow& window, Moveable* draggedMoveable);

    void draw(sf::RenderWindow& window);

    Moveable* getPlayer() const;

    Moveable* isOnMoveable(const Vector2f& position);
    bool containsMoveable(Moveable* moveable);

   private:
    void deleteMoveable(Moveable* moveable);

    static bool cornersInsideRectangle(const sf::RectangleShape& rect, const std::array<Vector2f, 4>& corners);
    static std::array<Vector2f, 4> findCorners(const Vector2f& position, const Vector2f& size);

    std::vector<sf::RectangleShape> staticRectangles_;
    std::vector<std::unique_ptr<Moveable>> moveables_;

    std::vector<Moveable*> killNextFrame_;

    int idCounter_;

    Moveable* player_;

    float movementSpeed_;
    float gravity_;

    const short rectangleLimit_ = 500;
    const short moveableLimit_ = 128;
};

#include "Moveable.h"
