#pragma once

#include "Utility/Timer.h"

#include "Player.h"

class GrowingPlayer : public Player {
    bool isGrowing_;
    Vector2f growthTarget_;

   public:
    explicit GrowingPlayer(SoundManager& soundManager);
    GrowingPlayer(Vector2f position, float playerSizeX, float playerSizeY, sf::Color playerColor, SoundManager& soundManager);

    void update(float elapsedTime, sf::RenderWindow& window, World& world) override;

    void onEnemyKill() override;

    MoveableType getType() override;

    std::string toString() override;
};
