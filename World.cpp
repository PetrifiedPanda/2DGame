#include "World.h"

World::World(const float movementSpeed, const float gravity) : idCounter_(0), movementSpeed_(movementSpeed), gravity_(gravity), player_(nullptr) {
    staticRectangles_.reserve(s_rectangleLimit);
    moveables_.reserve(s_moveableLimit);
    killNextFrame_.reserve(5);
}

void World::addMoveable(std::unique_ptr<Moveable>&& moveable) {
    if (moveables_.size() < static_cast<size_t>(s_moveableLimit)) {
        moveables_.push_back(std::move(moveable));
        moveables_[moveables_.size() - 1]->setID(idCounter_);
        soundManager.addEntitySound(idCounter_);
        ++idCounter_;
    }
}

void World::addRectangle(sf::RectangleShape&& rectangle) {
    if (staticRectangles_.size() < static_cast<size_t>(s_rectangleLimit))
        staticRectangles_.push_back(std::move(rectangle));
}

void World::addPlayer(std::unique_ptr<Moveable>&& player) {
    if (moveables_.size() < static_cast<size_t>(s_moveableLimit)) {
        player_ = player.get();
        moveables_.push_back(std::move(player));
        moveables_[moveables_.size() - 1]->setID(idCounter_);
        idCounter_++;
    }
}

void World::killMoveable(Moveable* moveable) {
#pragma omp critical
    killNextFrame_.push_back(moveable);
}

int World::getMoveableCount() const {
    return moveables_.size();
}

int World::getStaticRectangleCount() const {
    return staticRectangles_.size();
}

float World::getGravity() const {
    return gravity_;
}

float World::getMovementSpeed() const {
    return movementSpeed_;
}

void World::scale(sf::Vector2f scale) {
    movementSpeed_ *= scale.x;
    gravity_ *= scale.y;

    for (auto& rect : staticRectangles_) {
        sf::Vector2f size = rect.getSize();
        sf::Vector2f position = rect.getPosition();
        rect.setSize(sf::Vector2f(size.x * scale.x,
                                  size.y * scale.y));
        rect.setPosition(sf::Vector2f(position.x * scale.x,
                                      position.y * scale.y));
    }

    for (auto& moveable : moveables_) {
        sf::Vector2f size = moveable->getSize();
        sf::Vector2f position = moveable->getPosition();
        moveable->setSize(sf::Vector2f(size.x * scale.x,
                                       size.y * scale.y));
        moveable->setPosition(sf::Vector2f(position.x * scale.x,
                                           position.y * scale.y));
    }
}

// This is parts of an attempt to make alternative collision checks
sf::Vector2f World::getMaxMovement(Moveable* moveable, const sf::Vector2f& direction) {
    const sf::Vector2f size = moveable->getSize();
    const sf::RectangleShape& rect = moveable->getRectangle();

    float xDir = direction.x;
    float yDir = direction.y;

    const int staticRectSize = staticRectangles_.size();

    sf::Vector2f tmp = direction;

#pragma omp parallel for private(xDir, yDir)
    for (int i = 0; i < staticRectSize; ++i) {
        tmp = getDistanceUntilCollision(rect, staticRectangles_[i], direction);
        xDir = tmp.x;
        yDir = tmp.y;
    }

    const int moveablesSize = moveables_.size();

#pragma omp parallel for private(xDir, yDir)
    for (int i = 0; i < moveablesSize; ++i) {
        tmp = getDistanceUntilCollision(rect, moveables_[i]->getRectangle(), direction);
        xDir = tmp.x;
        yDir = tmp.y;
    }

    if (xDir > 0 && yDir > 0) {
        // min min
    } else if (xDir <= 0 && yDir <= 0) {
        // max max
    } else if (xDir > 0 && yDir <= 0) {
        // min max
    } else {
        // max min
    }

#pragma omp barrier

    return tmp;
}

sf::Vector2f World::getDistanceUntilCollision(const sf::RectangleShape& rect1, const sf::RectangleShape& rect2, const sf::Vector2f& direction) {
    const sf::Vector2f pos1 = rect1.getPosition();
    const sf::Vector2f sizePos1 = pos1 + rect1.getSize();

    const sf::Vector2f pos2 = rect2.getPosition();
    const sf::Vector2f sizePos2 = pos2 + rect2.getSize();

    sf::Vector2f result;

    if (sizePos1.x < pos2.x)
        result.x = pos2.x - sizePos1.x;
    else
        result.x = sizePos2.x - pos1.x;

    if (sizePos1.y < pos2.y)
        result.y = pos2.y - sizePos1.y;
    else
        result.y = sizePos2.y - pos1.x;

    if (direction.x < 0 && result.x > 0 || direction.x > 0 && result.x < 0)
        result.x = direction.x;

    if (direction.y < 0 && result.y > 0 || direction.y > 0 && result.y < 0)
        result.y = direction.y;

    if (direction.x == 0)
        result.x = 0;

    if (direction.y == 0)
        result.y = 0;

    return result;
}

bool World::canMoveInDirection(Moveable* moveable, const sf::Vector2f& direction) {
    const sf::Vector2f newPosition = moveable->getPosition() + direction;
    const sf::Vector2f size = moveable->getSize();

    const std::array<sf::Vector2f, 4> corners = findCorners(newPosition, size);

    // Check collision with staticRectangles
    const int staticRectSize = staticRectangles_.size();
    bool foundCollision = false;

#pragma omp parallel for reduction(|| \
                                   : foundCollision)
    for (int i = 0; i < staticRectSize; ++i)
        if (doRectanglesOverlap(staticRectangles_[i], size, corners))
            foundCollision = true;

    if (foundCollision)
        return false;

    // Check collision with enemies
    const int moveablesSize = moveables_.size();

#pragma omp parallel for reduction(|| \
                                   : foundCollision)
    for (int i = 0; i < moveablesSize; ++i) {
        const sf::RectangleShape& rect = moveables_[i]->getRectangle();

        if (moveables_[i].get() == moveable)
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
    for (Moveable* moveable : killNextFrame_)
        deleteMoveable(moveable);

    killNextFrame_.clear();

    for (auto& moveable : moveables_) {
        if (moveable.get() != draggedMoveable) {
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
            return moveable.get();
    }

    return nullptr;
}

bool World::containsMoveable(Moveable* moveable) {
    for (auto& ptr : moveables_) {
        if (ptr.get() == moveable)
            return true;
    }

    return false;
}

void World::deleteMoveable(Moveable* moveable) {
    for (auto it = moveables_.begin(); it != moveables_.end(); ++it) {
        if (it->get() == moveable) {
            moveables_.erase(it);
            break;
        }
    }
}
