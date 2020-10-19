#include "GrowingPlayer.h"

GrowingPlayer::GrowingPlayer(SoundManager& soundManager) : Player(soundManager), isGrowing_(false), growthTarget_(0, 0) {}

GrowingPlayer::GrowingPlayer(const Vector2f& position, const sf::Vector2f& size, const sf::Color playerColor, SoundManager& soundManager)
    : Player(position, size, playerColor, soundManager), isGrowing_(false), growthTarget_(0, 0) {}

void GrowingPlayer::update(const float elapsedTime, sf::RenderWindow& window, World& world) {
    if (isGrowing_) {
        const Vector2f size = getSize();
        if (size == growthTarget_) {
            growthTarget_ = Vector2f(0, 0);
            isGrowing_ = false;
        } else {
            const float movementSpeed = world.getMovementSpeed();

            const float growthFactorX = movementSpeed / 10.0f * elapsedTime;
            const float growthFactorY = movementSpeed / 10.0f * elapsedTime;

            float newSizeX = growthFactorX + size.x;
            float newSizeY = growthFactorY + size.y;

            if (newSizeX > growthTarget_.x)
                newSizeX = growthTarget_.x;

            if (newSizeY > growthTarget_.y)
                newSizeY = growthTarget_.y;

            const Vector2f position = getPosition();
            setSize(Vector2f(newSizeX, newSizeY));
            move(Vector2f(-growthFactorX, -growthFactorY));

            if (!world.canMoveInDirection(this, Vector2f(0, 0))) {
                setSize(size);
                setPosition(position);
            }
        }
    }

    Player::update(elapsedTime, window, world);
}

void GrowingPlayer::onEnemyKill() {
    if (isGrowing_) {
        growthTarget_ = growthTarget_ + Vector2f(growthTarget_.x * 0.1f, growthTarget_.y * 0.1f);
    } else {
        isGrowing_ = true;
        const Vector2f size = getSize();
        const float growthX = size.x * 0.1f;
        const float growthY = size.y * 0.1f;
        growthTarget_ = Vector2f(size.x + growthX, size.y + growthY);
    }
}

MoveableType GrowingPlayer::getType() {
    return MoveableType::GROWINGPLAYER;
}

std::string GrowingPlayer::toString() {
    return Player::toString() +
           "isGrowing = " + std::to_string(isGrowing_) + "\n" +
           "growthTarget = x: " + std::to_string(growthTarget_.x) + " y: " + std::to_string(growthTarget_.y) + "\n";
}
