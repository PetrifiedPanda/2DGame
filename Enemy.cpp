#include "Enemy.h"

Enemy::Enemy(SoundManager& soundManager) : Moveable(soundManager) {
    hitBox_ = sf::RectangleShape();

    movingLeft_ = false;
}

Enemy::Enemy(const Vector2f position, const float playerSizeX, const float playerSizeY, const sf::Color enemyColor, SoundManager& soundManager) : Moveable(soundManager) {
    setFillColor(enemyColor);
    hitBox_ = sf::RectangleShape();
    Enemy::setSize(Vector2f(playerSizeX, playerSizeY));
    Enemy::setPosition(position);

    movingLeft_ = false;
}

void Enemy::update(const float elapsedTime, sf::RenderWindow& window, World& world) {
    if (isAttacked(world.getPlayer())) {
        soundManager_.playDeathSound(getID());
        die(world);
        dynamic_cast<Player*>(world.getPlayer())->onEnemyKill();
        return;
    }

    Vector2f direction(0, 0);
    const float movementDistance = world.getMovementSpeed() / 2.0f * elapsedTime;

    if (movingLeft_)
        direction += Vector2f(-movementDistance, 0);
    else
        direction += Vector2f(movementDistance, 0);

    if (world.canMoveInDirection(this, direction)) {
        move(direction);
    } else {
        movingLeft_ = !movingLeft_;
        soundManager_.playCollisionSound(getID());
    }

    Moveable::update(elapsedTime, window, world);
}

MoveableType Enemy::getType() {
    return MoveableType::ENEMY;
}

std::string Enemy::toString() {
    return Moveable::toString() +
           "movingLeft = " + std::to_string(movingLeft_) + "\n";
}

void Enemy::setPosition(const Vector2f& position) {
    const Vector2f difference = getPosition() - position;
    hitBox_.move(-difference);
    Moveable::setPosition(position);
}

void Enemy::move(const Vector2f& direction) {
    hitBox_.move(direction);
    Moveable::move(direction);
}

void Enemy::setSize(const Vector2f& size) {
    Moveable::setSize(size);
    repositionHitBox();
}

void Enemy::repositionHitBox() {
    const Vector2f position = getPosition();
    const Vector2f size = getSize();

    const float offset = size.y / 10;
    hitBox_.setSize(Vector2f(size.x / 1.5f, offset));
    const float xPos = position.x + size.x / 2 - hitBox_.getSize().x / 2;
    hitBox_.setPosition(Vector2f(xPos, position.y - offset));
}

bool Enemy::isAttacked(Moveable* player) const {
    if (World::doRectanglesOverlap(hitBox_, player->getRectangle()))
        return true;
    else
        return false;
}
