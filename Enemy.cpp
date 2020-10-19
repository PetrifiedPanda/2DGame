#include "Enemy.h"

Enemy::Enemy() : Moveable(), hitBox_(), movingLeft_(false) {}

Enemy::Enemy(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Color enemyColor)
    : Moveable(), hitBox_(), movingLeft_(false) {
    setFillColor(enemyColor);
    Enemy::setSize(size);
    Enemy::setPosition(position);
}

void Enemy::update(const float elapsedTime, sf::RenderWindow& window, World& world) {
    if (isAttacked(world.getPlayer())) {
        world.soundManager.playDeathSound(getID());
        die(world);
        dynamic_cast<Player*>(world.getPlayer())->onEnemyKill();
        return;
    }

    sf::Vector2f direction(0, 0);
    const float movementDistance = world.getMovementSpeed() / 2.0f * elapsedTime;

    if (movingLeft_)
        direction += sf::Vector2f(-movementDistance, 0);
    else
        direction += sf::Vector2f(movementDistance, 0);

    if (world.canMoveInDirection(this, direction)) {
        move(direction);
    } else {
        movingLeft_ = !movingLeft_;
        world.soundManager.playCollisionSound(getID());
    }
}

MoveableType Enemy::getType() {
    return MoveableType::ENEMY;
}

std::string Enemy::toString() {
    return Moveable::toString() +
           "movingLeft = " + std::to_string(movingLeft_) + "\n";
}

void Enemy::setPosition(const sf::Vector2f& position) {
    const sf::Vector2f difference = getPosition() - position;
    hitBox_.move(-difference);
    Moveable::setPosition(position);
}

void Enemy::move(const sf::Vector2f& direction) {
    hitBox_.move(direction);
    Moveable::move(direction);
}

void Enemy::setSize(const sf::Vector2f& size) {
    Moveable::setSize(size);
    repositionHitBox();
}

void Enemy::repositionHitBox() {
    const sf::Vector2f position = getPosition();
    const sf::Vector2f size = getSize();

    const float offset = size.y / 10;
    hitBox_.setSize(sf::Vector2f(size.x / 1.5f, offset));
    const float xPos = position.x + size.x / 2 - hitBox_.getSize().x / 2;
    hitBox_.setPosition(sf::Vector2f(xPos, position.y - offset));
}

bool Enemy::isAttacked(Moveable* player) const {
    if (World::doRectanglesOverlap(hitBox_, player->getRectangle()))
        return true;
    else
        return false;
}
