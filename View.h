#pragma once

#include "SFML/Graphics.hpp"

#include "Player.h"

using sf::Vector2f;

class View {
   public:
    View(sf::RenderWindow& window, Player* player, float scrollingSpeed);

    void update(float elapsedTime);

    sf::View getWindowView() const;
    sf::View getStatView() const;

    Vector2f getPositionInGame(const Vector2f& positionOnWindow) const;
    Vector2f getPositionOnWindow(const Vector2f& positionInGame) const;
    Vector2f getViewDifference() const;

   private:
    Player* player_;

    Vector2f prevPlayerPos_;

    sf::View windowView_;
    sf::View statView_;

    Vector2f viewDifference_;

    int windowWidth_;
    int windowHeight_;

    float scrollingSpeed_;
    float originalPlayerSize_;
};
