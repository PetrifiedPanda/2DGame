#include "World.h"

World::World(const float movementSpeed, const float gravity) : idCounter_(0), movementSpeed_(movementSpeed), gravity_(gravity), player_(nullptr) {
    staticRectangles_.reserve(s_rectangleLimit);
    moveables_.reserve(s_moveableLimit);
    killNextFrame_.reserve(5);
}

World::~World() {
    for (Moveable* moveable : moveables_)
        delete moveable;
}

void World::addMoveable(Moveable* moveable) {
    if (moveables_.size() < s_moveableLimit) {
        moveables_.push_back(moveable);
        moveables_[moveables_.size() - 1]->setID(idCounter_);
        soundManager.addEntitySound(idCounter_);
        ++idCounter_;
    }
}

void World::addRectangle(sf::RectangleShape&& rectangle) {
    if (staticRectangles_.size() < s_rectangleLimit)
        staticRectangles_.push_back(std::move(rectangle));
}

void World::addPlayer(Moveable* player) {
    if (moveables_.size() < s_moveableLimit) {
        player_ = player;
        moveables_.push_back(player);
        moveables_[moveables_.size() - 1]->setID(idCounter_);
        ++idCounter_;
    }
}

void World::killMoveable(Moveable* moveable) {
    killNextFrame_.push_back(moveable);
    soundManager.removeEntity(moveable->id_);
}

size_t World::getMoveableCount() const {
    return moveables_.size();
}

size_t World::getStaticRectangleCount() const {
    return staticRectangles_.size();
}

float World::getGravity() const {
    return gravity_;
}

float World::getMovementSpeed() const {
    return movementSpeed_;
}

void World::scale(const sf::Vector2f& scale) {
    movementSpeed_ *= scale.x;
    gravity_ *= scale.y;

    for (sf::RectangleShape& rect : staticRectangles_) {
        sf::Vector2f size = rect.getSize();
        sf::Vector2f position = rect.getPosition();
        rect.setSize(sf::Vector2f(size.x * scale.x,
                                  size.y * scale.y));
        rect.setPosition(sf::Vector2f(position.x * scale.x,
                                      position.y * scale.y));
    }

    for (Moveable* moveable : moveables_) {
        sf::Vector2f size = moveable->getSize();
        sf::Vector2f position = moveable->getPosition();
        moveable->setSize(sf::Vector2f(size.x * scale.x,
                                       size.y * scale.y));
        moveable->setPosition(sf::Vector2f(position.x * scale.x,
                                           position.y * scale.y));
    }
}

bool World::canMoveInDirection(Moveable* moveable, const sf::Vector2f& direction) {
    const sf::Vector2f newPosition = moveable->getPosition() + direction;
    const sf::Vector2f size = moveable->getSize();

    const std::array<sf::Vector2f, 4> corners = findCorners(newPosition, size);

    // Check collision with staticRectangles
    const int staticRectSize = staticRectangles_.size();
    bool foundCollision = false;

    #pragma omp parallel for reduction(||: foundCollision)
    for (int i = 0; i < staticRectSize; ++i)
        if (doRectanglesOverlap(staticRectangles_[i], size, corners))
            foundCollision = true;

    if (foundCollision)
        return false;

    // Check collision with enemies
    const int moveablesSize = moveables_.size();

    #pragma omp parallel for reduction(||: foundCollision)
    for (int i = 0; i < moveablesSize; ++i) {
        const sf::RectangleShape& rect = moveables_[i]->getRectangle();

        if (moveables_[i] == moveable)
            continue;

        if (doRectanglesOverlap(rect, size, corners))
            foundCollision = true;
    }

    if (foundCollision)
        return false;
    else
        return true;
}

bool World::doRectanglesOverlap(const sf::RectangleShape& rect, const sf::Vector2f& size, const std::array<sf::Vector2f, 4>& corners) {
    const sf::Vector2f rectSize = rect.getSize();

    // if rect is larger than the other rectangle, check if corners of the rectangle are inside the other
    if (rectSize.x > size.x || rectSize.y > size.y) {
        if (cornersInsideRectangle(rect, corners))
            return true;
    }

    // Check if any of the corners of rect are inside the other Rectangle
    const std::array<sf::Vector2f, 4> rectCorners = findCorners(rect.getPosition(), rectSize);

    const sf::Vector2f position = corners[0];

    const float rightX = corners[3].x;
    const float downY = corners[3].y;

    for (const sf::Vector2f& corner : rectCorners) {
        if (position.x <= corner.x && rightX >= corner.x &&
            position.y <= corner.y && downY >= corner.y)
            return true;
    }

    return false;
}

bool World::doRectanglesOverlap(const sf::RectangleShape& rect, const sf::RectangleShape& otherRect) {
    const sf::Vector2f rectSize = rect.getSize();
    const sf::Vector2f otherRectSize = otherRect.getSize();

    if (rectSize.x > otherRectSize.x || rectSize.y > otherRectSize.y) {
        const std::array<sf::Vector2f, 4> otherRectCorners = findCorners(otherRect.getPosition(), otherRectSize);

        if (cornersInsideRectangle(rect, otherRectCorners))
            return true;
    }

    const std::array<sf::Vector2f, 4> rectCorners = findCorners(rect.getPosition(), rectSize);

    if (cornersInsideRectangle(otherRect, rectCorners))
        return true;
    else
        return false;
}

bool World::cornersInsideRectangle(const sf::RectangleShape& rect, const std::array<sf::Vector2f, 4>& corners) {
    const sf::Vector2f rectPosition = rect.getPosition();
    const sf::Vector2f rectSize = rect.getSize();

    const float rightX = rectPosition.x + rectSize.x;
    const float downY = rectPosition.y + rectSize.y;

    for (const sf::Vector2f& corner : corners) {
        if (rectPosition.x <= corner.x && rightX >= corner.x &&
            rectPosition.y <= corner.y && downY >= corner.y)
            return true;
    }

    return false;
}

std::array<sf::Vector2f, 4> World::findCorners(const sf::Vector2f& position, const sf::Vector2f& size) {
    std::array<sf::Vector2f, 4> corners;

    const float rightX = position.x + size.x;
    const float downY = position.y + size.y;

    corners[0] = position;
    corners[1] = sf::Vector2f(rightX, position.y);
    corners[2] = sf::Vector2f(position.x, downY);
    corners[3] = sf::Vector2f(rightX, downY);

    return corners;
}

void World::draw(sf::RenderWindow& window) {
    for (const sf::RectangleShape& shape : staticRectangles_)
        window.draw(shape);

    for (auto& moveable : moveables_)
        window.draw(moveable->getRectangle());
}

Moveable* World::getPlayer() const {
    return player_;
}

void World::update(const float elapsedTime, sf::RenderWindow& window, Moveable* draggedMoveable) {
    if (!killNextFrame_.empty())
        deleteKilledMoveables();

    for (auto& moveable : moveables_) {
        if (moveable != draggedMoveable) {
            moveable->update(elapsedTime, window, *this);
            moveable->gravityUpdate(elapsedTime, window, *this);
        }
    }
}

Moveable* World::isOnMoveable(const sf::Vector2f& position) {
    for (auto& moveable : moveables_) {
        const sf::Vector2f upperLeft = moveable->getPosition();
        const sf::Vector2f lowerRight = upperLeft + moveable->getSize();

        if (position.x >= upperLeft.x && position.x <= lowerRight.x && position.y >= upperLeft.y && position.y <= lowerRight.y)
            return moveable;
    }

    return nullptr;
}

bool World::containsMoveable(Moveable* moveable) {
    for (auto& ptr : moveables_) {
        if (ptr == moveable)
            return true;
    }

    return false;
}

void World::deleteMoveable(Moveable* moveable) {
    for (auto it = moveables_.begin(); it != moveables_.end(); ++it) {
        if (*it == moveable) {
            delete *it;
            moveables_.erase(it);
            break;
        }
    }
}

void World::deleteKilledMoveables() {
    auto currentKilledIt = killNextFrame_.begin();
    auto moveablesIt = moveables_.begin();
    while (currentKilledIt != killNextFrame_.end()) {
        while (*moveablesIt != *currentKilledIt)
            ++moveablesIt;
        delete *moveablesIt;
        moveablesIt = moveables_.erase(moveablesIt);
        ++currentKilledIt;
    }

    killNextFrame_.clear();
}