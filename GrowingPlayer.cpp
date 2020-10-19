#include "GrowingPlayer.h"

GrowingPlayer::GrowingPlayer() : Player(), isGrowing_(false), growthTarget_(0, 0) {}

GrowingPlayer::GrowingPlayer(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Color playerColor)
    : Player(position, size, playerColor), isGrowing_(false), growthTarget_(0, 0) {}

void GrowingPlayer::update(const float elapsedTime, sf::RenderWindow& window, World& world) {
    if (isGrowing_) {
        const sf::Vector2f size = getSize();
        if (size == growthTarget_) {
            growthTarget_ = sf::Vector2f(0, 0);
            isGrowing_ = false;
        } else {
            const float growthFactor = world.getMovementSpeed() / 10.0f * elapsedTime;

            float newSizeX = growthFactor + size.x;
            float newSizeY = growthFactor + size.y;

            if (newSizeX > growthTarget_.x)
                newSizeX = growthTarget_.x;

            if (newSizeY > growthTarget_.y)
                newSizeY = growthTarget_.y;

            const sf::Vector2f oldPosition = getPosition();
            setSize(sf::Vector2f(newSizeX, newSizeY));
            move(sf::Vector2f(-growthFactor, -growthFactor));

            if (!world.canMoveInDirection(this, sf::Vector2f(0, 0))) {
                setSize(size);
                setPosition(oldPosition);
            }
        }
    }

    Player::update(elapsedTime, window, world);
}

void GrowingPlayer::onEnemyKill() {
    if (isGrowing_) {
        growthTarget_ = growthTarget_ + sf::Vector2f(growthTarget_.x * 0.1f, growthTarget_.y * 0.1f);
    } else {
        isGrowing_ = true;
        const sf::Vector2f size = getSize();
        const float growthX = size.x * 0.1f;
        const float growthY = size.y * 0.1f;
        growthTarget_ = sf::Vector2f(size.x + growthX, size.y + growthY);
    }
}

MoveableType GrowingPlayer::getType() const {
    return MoveableType::GROWINGPLAYER;
}

std::string GrowingPlayer::toString() const {
    return Player::toString() +
           "isGrowing = " + std::to_string(isGrowing_) + "\n" +
           "growthTarget = x: " + std::to_string(growthTarget_.x) + " y: " + std::to_string(growthTarget_.y) + "\n";
}
