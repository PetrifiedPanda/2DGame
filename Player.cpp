#include "Player.h"

Player::Player(SoundManager& soundManager) : Moveable(soundManager) {
    movingUp_ = movingDown_ = movingLeft_ = movingRight_ = jumping_ = false;
    jumpHeight_ = 0.0f;
    maxJumpHeight_ = getSize().y * 7;
}

Player::Player(const Vector2f& position, const float sizeX, const float sizeY, const sf::Color playerColor, SoundManager& soundManager) : Moveable(position, soundManager) {
    setFillColor(playerColor);
    Moveable::setSize(Vector2f(sizeX, sizeY));
    setOutlineColor(sf::Color::Green);
    setOutlineThickness(sizeX / 10);

    movingUp_ = movingDown_ = movingLeft_ = movingRight_ = jumping_ = false;
    jumpHeight_ = 0.0f;
    maxJumpHeight_ = getSize().y * 7;
}

void Player::update(const float elapsedTime, sf::RenderWindow& window, World& world) {
    const Vector2f gravityDirection(0, world.getGravity() * elapsedTime);
    bool onGround;

    if (world.canMoveInDirection(this, gravityDirection))
        onGround = false;
    else
        onGround = true;

    if (window.hasFocus()) {
        if (world.getGravity() == 0.0f) {
            if (sf::Keyboard::isKeyPressed(g_upKey))
                movingUp_ = true;
            else
                movingUp_ = false;

            if (sf::Keyboard::isKeyPressed(g_downKey))
                movingDown_ = true;
            else
                movingDown_ = false;
        }

        if (sf::Keyboard::isKeyPressed(g_leftKey))
            movingLeft_ = true;
        else
            movingLeft_ = false;

        if (sf::Keyboard::isKeyPressed(g_rightKey))
            movingRight_ = true;
        else
            movingRight_ = false;

        if (sf::Keyboard::isKeyPressed(g_jumpKey)) {
            if (onGround)
                jumping_ = true;
        } else {
            jumping_ = false;
            jumpHeight_ = 0.0f;
        }
    }

    const float movementDistance = world.getMovementSpeed() * elapsedTime;

    float verticalDirection = 0.0f;
    float horizontalDirection = 0.0f;

    if (movingUp_)
        verticalDirection -= movementDistance;

    if (movingDown_)
        verticalDirection += movementDistance;

    if (movingLeft_)
        horizontalDirection -= movementDistance;

    if (movingRight_)
        horizontalDirection += movementDistance;

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

    const Vector2f verticalDirectionVector(0, verticalDirection);

    if (world.canMoveInDirection(this, verticalDirectionVector)) {
        move(verticalDirectionVector);
    } else {
        soundManager_.playCollisionSound(getID());
        jumping_ = false;  // Reset jump if you can't go further up
        jumpHeight_ = 0;
    }

    const Vector2f horizontalDirectionVector(horizontalDirection, 0);

    if (world.canMoveInDirection(this, horizontalDirectionVector))
        move(horizontalDirectionVector);
    else
        soundManager_.playCollisionSound(getID());

    Moveable::update(elapsedTime, window, world);
}

void Player::onEnemyKill() {
}

MoveableType Player::getType() {
    return MoveableType::PLAYER;
}

std::string Player::toString() {
    return Moveable::toString() +
           "jumpHeight = " + std::to_string(jumpHeight_) + "\n" +
           "jumping = " + std::to_string(jumping_) + "\n" +
           "movingUp = " + std::to_string(movingUp_) + "\n" +
           "movingDown = " + std::to_string(movingDown_) + "\n" +
           "movingLeft = " + std::to_string(movingLeft_) + "\n" +
           "movingRight = " + std::to_string(movingRight_) + "\n";
}