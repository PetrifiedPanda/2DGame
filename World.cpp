#include "World.h"

World::World() : idCounter_(0) {
    movementSpeed_ = 100;
    gravity_ = 0;
    player_ = nullptr;

    staticRectangles_.reserve(rectangleLimit_);
    moveables_.reserve(moveableLimit_);
    killNextFrame_.reserve(5);
}

World::World(const float movementSpeed, const float gravity) : World() {
    movementSpeed_ = movementSpeed;
    gravity_ = gravity;
}

void World::addMoveable(std::unique_ptr<Moveable>&& moveable) {
    if (moveables_.size() < static_cast<size_t>(moveableLimit_)) {
        moveables_.push_back(std::move(moveable));
        moveables_[moveables_.size() - 1]->setID(idCounter_);
        idCounter_++;
    }
}

void World::addRectangle(const sf::RectangleShape& rectangle) {
    if (staticRectangles_.size() < static_cast<size_t>(rectangleLimit_))
        staticRectangles_.push_back(rectangle);
}

void World::addPlayer(std::unique_ptr<Moveable>&& player) {
    if (moveables_.size() < static_cast<size_t>(moveableLimit_)) {
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

Vector2f World::getMaxMovement(Moveable* moveable, const Vector2f& direction) {
    const Vector2f size = moveable->getSize();
    const sf::RectangleShape rect = moveable->getRectangle();

    float xDir = direction.x;
    float yDir = direction.y;

    const int staticRectSize = staticRectangles_.size();

    Vector2f tmp = direction;

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

Vector2f World::getDistanceUntilCollision(const sf::RectangleShape& rect1, const sf::RectangleShape& rect2, const Vector2f& direction) {
    const Vector2f pos1 = rect1.getPosition();
    const Vector2f sizePos1 = pos1 + rect1.getSize();

    const Vector2f pos2 = rect2.getPosition();
    const Vector2f sizePos2 = pos2 + rect2.getSize();

    Vector2f result;

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

bool World::canMoveInDirection(Moveable* moveable, const Vector2f& direction) {
    const Vector2f newPosition = moveable->getPosition() + direction;
    const Vector2f size = moveable->getSize();

    const std::array<Vector2f, 4> corners = findCorners(newPosition, size);

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
        sf::RectangleShape rect = moveables_[i]->getRectangle();

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

bool World::doRectanglesOverlap(const sf::RectangleShape& rect, const Vector2f& size, const std::array<Vector2f, 4>& corners) {
    const Vector2f rectSize = rect.getSize();

    // if rect is larger than the other rectangle, check if corners of the rectangle are inside the other
    if (rectSize.x > size.x || rectSize.y > size.y) {
        if (cornersInsideRectangle(rect, corners))
            return true;
    }

    // Check if any of the corners of rect are inside the other Rectangle
    const std::array<Vector2f, 4> rectCorners = findCorners(rect.getPosition(), rectSize);

    const Vector2f position = corners[0];

    const float rightX = corners[3].x;
    const float downY = corners[3].y;

    for (const Vector2f& corner : rectCorners) {
        if (position.x <= corner.x && rightX >= corner.x &&
            position.y <= corner.y && downY >= corner.y)
            return true;
    }

    return false;
}

bool World::doRectanglesOverlap(const sf::RectangleShape& rect, const sf::RectangleShape& otherRect) {
    const Vector2f rectSize = rect.getSize();
    const Vector2f otherRectSize = otherRect.getSize();

    if (rectSize.x > otherRectSize.x || rectSize.y > otherRectSize.y) {
        const std::array<Vector2f, 4> otherRectCorners = findCorners(otherRect.getPosition(), otherRectSize);

        if (cornersInsideRectangle(rect, otherRectCorners))
            return true;
    }

    const std::array<Vector2f, 4> rectCorners = findCorners(rect.getPosition(), rectSize);

    if (cornersInsideRectangle(otherRect, rectCorners))
        return true;
    else
        return false;
}

bool World::cornersInsideRectangle(const sf::RectangleShape& rect, const std::array<Vector2f, 4>& corners) {
    const Vector2f rectPosition = rect.getPosition();
    const Vector2f rectSize = rect.getSize();

    const float rightX = rectPosition.x + rectSize.x;
    const float downY = rectPosition.y + rectSize.y;

    for (const Vector2f& corner : corners) {
        if (rectPosition.x <= corner.x && rightX >= corner.x &&
            rectPosition.y <= corner.y && downY >= corner.y)
            return true;
    }

    return false;
}

std::array<Vector2f, 4> World::findCorners(const Vector2f& position, const Vector2f& size) {
    std::array<Vector2f, 4> corners;

    const float rightX = position.x + size.x;
    const float downY = position.y + size.y;

    corners[0] = position;
    corners[1] = Vector2f(rightX, position.y);
    corners[2] = Vector2f(position.x, downY);
    corners[3] = Vector2f(rightX, downY);

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

    const int size = moveables_.size();

    for (auto& moveable : moveables_) {
        if (moveable.get() != draggedMoveable)
            moveable->update(elapsedTime, window, *this);
    }
}

Moveable* World::isOnMoveable(const Vector2f& position) {
    for (auto& moveable : moveables_) {
        const Vector2f upperLeft = moveable->getPosition();
        const Vector2f lowerRight = upperLeft + moveable->getSize();

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
