#include "Player.h"

Player::Player(SoundManager& soundManager) : Moveable(soundManager), jumping_(false), jumpHeight_(0.0f), maxJumpHeight_(getSize().y * 7) {}

Player::Player(const sf::Vector2f& position, const sf::Vector2f& size, const sf::Color playerColor, SoundManager& soundManager)
    : Moveable(soundManager, position, size), jumping_(false), jumpHeight_(0.0f), maxJumpHeight_(getSize().y * 7) {
    setFillColor(playerColor);
    setOutlineColor(sf::Color::Green);
    setOutlineThickness(size.x / 10);
}

void Player::update(const float elapsedTime, sf::RenderWindow& window, World& world) {
    const sf::Vector2f gravityDirection(0, world.getGravity() * elapsedTime);
    bool onGround;

    if (world.canMoveInDirection(this, gravityDirection))
        onGround = false;
    else
        onGround = true;

    const float movementDistance = world.getMovementSpeed() * elapsedTime;

    float verticalDirection = 0.0f;
    float horizontalDirection = 0.0f;

    if (window.hasFocus()) {
        if (world.getGravity() == 0.0f) {
            if (sf::Keyboard::isKeyPressed(g_upKey))
                verticalDirection -= movementDistance;

            if (sf::Keyboard::isKeyPressed(g_downKey))
                verticalDirection += movementDistance;
        }

        if (sf::Keyboard::isKeyPressed(g_leftKey))
            horizontalDirection -= movementDistance;

        if (sf::Keyboard::isKeyPressed(g_rightKey))
            horizontalDirection += movementDistance;

        if (sf::Keyboard::isKeyPressed(g_jumpKey)) {
            if (onGround)
                jumping_ = true;
        } else {
            jumping_ = false;
            jumpHeight_ = 0.0f;
        }
    }

    if (jumping_) {
        const float jumpDirection = movementDistance * 2.0f + gravityDirection.y;

        verticalDirection -= jumpDirection;

        // Check for maximum jump height
        if (jumpHeight_ < maxJumpHeight_) {
            jumpHeight_ -= verticalDirection + gravityDirection.y;
        } else {
            verticalDirection += jumpDirection;
            jumpHeight_ = 0.0f;
            jumping_ = false;
        }
    }

    // Check vertical and Horizontal direction separately

    const sf::Vector2f verticalDirectionVector(0, verticalDirection);

    if (world.canMoveInDirection(this, verticalDirectionVector)) {
        move(verticalDirectionVector);
    } else {
        soundManager_.playCollisionSound(getID());
        jumping_ = false;  // Reset jump if you can't go further up
        jumpHeight_ = 0;
    }

    const sf::Vector2f horizontalDirectionVector(horizontalDirection, 0);

    if (world.canMoveInDirection(this, horizontalDirectionVector))
        move(horizontalDirectionVector);
    else
        soundManager_.playCollisionSound(getID());
}

void Player::onEnemyKill() {
}

MoveableType Player::getType() {
    return MoveableType::PLAYER;
}

std::string Player::toString() {
    return Moveable::toString() +
           "jumpHeight = " + std::to_string(jumpHeight_) + "\n" +
           "jumping = " + std::to_string(jumping_) + "\n";
}