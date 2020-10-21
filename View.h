#pragma once

#include "SFML/Graphics.hpp"

#include "Moveable.h"

using sf::Vector2f;

class View {
    int windowWidth_;
    int windowHeight_;

    sf::View windowView_;
    sf::View statView_;

    Vector2f viewDifference_;

    Moveable* followed_;
    Vector2f prevPlayerPos_;

    float scrollingSpeed_;
    sf::Vector2f originalPlayerSize_;

   public:
    View(sf::RenderWindow& window, Moveable* followed, float scrollingSpeed);

    void update(float elapsedTime);

    void windowResize(sf::RenderWindow& window, sf::Vector2f scale);

    const sf::View& getWindowView() const;
    const sf::View& getStatView() const;

    Vector2f getPositionInGame(const Vector2f& positionOnWindow) const;
    Vector2f getPositionOnWindow(const Vector2f& positionInGame) const;
    Vector2f getViewDifference() const;
};
