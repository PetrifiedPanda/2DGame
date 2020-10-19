#include "Moveable.h"

Moveable::Moveable(SoundManager& soundManager) : soundManager_(soundManager), id_(idNullValue) {
    rectangle_ = sf::RectangleShape();
}

Moveable::Moveable(const Vector2f& position, SoundManager& soundManager) : soundManager_(soundManager), id_(idNullValue) {
    rectangle_ = sf::RectangleShape();
    Moveable::setPosition(position);
}

Moveable::~Moveable() = default;

void Moveable::gravityUpdate(float elapsedTime, sf::RenderWindow& window, World& world) {
    const Vector2f gravityDirection(0, elapsedTime * world.getGravity());
    if (world.canMoveInDirection(this, gravityDirection))
        move(gravityDirection);

    float movementDistance = elapsedTime * world.getMovementSpeed();

    if (force_.x > 0.0f) {
        const Vector2f rightMovement(force_.x < movementDistance ? force_.x : movementDistance, 0.0f);

        if (world.canMoveInDirection(this, rightMovement)) {
            move(rightMovement);
            force_ -= rightMovement;
        } else
            force_.x = 0.0f;
    } else if (force_.x < 0.0f) {
        const Vector2f leftMovement(force_.x > -movementDistance ? force_.x : -movementDistance, 0.0f);

        if (world.canMoveInDirection(this, leftMovement)) {
            move(leftMovement);
            force_ -= leftMovement;
        } else
            force_.x = 0.0f;
    }

    if (force_.y > 0.0f) {
        const Vector2f downMovement(0.0f, force_.y < movementDistance ? force_.y : movementDistance);

        if (world.canMoveInDirection(this, downMovement)) {
            move(downMovement);
            force_ -= downMovement;
        } else
            force_.y = 0.0f;
    } else if (force_.y < 0.0f) {
        Vector2f upMovement = Vector2f(0.0f, force_.y > -movementDistance ? force_.y : -movementDistance);
        upMovement.y = upMovement.y * 2.0f - gravityDirection.y;

        if (world.canMoveInDirection(this, upMovement)) {
            move(upMovement);
            force_.y += movementDistance;
        } else
            force_.y = 0.0f;
    }
}

void Moveable::addForce(const Vector2f& force) {
    force_ += force;
}

void Moveable::die(World& world) {
    soundManager_.removeEntity(id_);
    world.killMoveable(this);
}

sf::RectangleShape Moveable::getRectangle() const {
    return rectangle_;
}

Vector2f Moveable::getSize() const {
    return rectangle_.getSize();
}

Vector2f Moveable::getPosition() const {
    return rectangle_.getPosition();
}

void Moveable::setPosition(const Vector2f& position) {
    rectangle_.setPosition(position);
}

void Moveable::setTexture(sf::Texture* texture) {
    rectangle_.setTexture(texture, true);
}

std::string Moveable::getTypeString() {
    const MoveableType type = getType();

    switch (type) {
        case MoveableType::PLAYER:
            return "Player";
        case MoveableType::GROWINGPLAYER:
            return "GrowingPlayer";
        case MoveableType::ENEMY:
            return "Enemy";
        default:
            return "Moveable";
    }
}

int Moveable::getID() const {
    return id_;
}

void Moveable::setID(int newID) {
    if (id_ != idNullValue)  // Making sure the id only gets changed once
        throw std::runtime_error("ID may not be changed");

    id_ = newID;
    soundManager_.addEntitySound(id_);
}

std::string Moveable::toString() {
    return "ID = " + std::to_string(getID()) + "\n" +
           "Type = " + getTypeString() + "\n" +
           "Position = x: " + std::to_string(getPosition().x) + " y: " + std::to_string(getPosition().y) + "\n" +
           "Size = x: " + std::to_string(getSize().x) + " y: " + std::to_string(getSize().y) + "\n" +
           "force = x: " + std::to_string(force_.x) + " y: " + std::to_string(force_.y) + "\n";
}

void Moveable::move(const Vector2f& direction) {
    rectangle_.move(direction);
}

void Moveable::setSize(const Vector2f& size) {
    rectangle_.setSize(size);
}

void Moveable::setFillColor(const sf::Color& color) {
    rectangle_.setFillColor(color);
}

void Moveable::setOutlineColor(const sf::Color& color) {
    rectangle_.setOutlineColor(color);
}

void Moveable::setOutlineThickness(const float thickness) {
    rectangle_.setOutlineThickness(thickness);
}
