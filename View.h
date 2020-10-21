#pragma once

#include "SFML/Graphics.hpp"

#include "Moveable.h"

class View {
    int windowWidth_;
    int windowHeight_;

    sf::View windowView_;
    sf::View statView_;

    sf::Vector2f viewDifference_;

    Moveable* followed_;
    sf::Vector2f prevPlayerPos_;

    float scrollingSpeed_;
    sf::Vector2f originalPlayerSize_;

   public:
    View(sf::RenderWindow& window, Moveable* followed, float scrollingSpeed);

    void update(float elapsedTime);

    void windowResize(sf::RenderWindow& window, sf::Vector2f scale);

    const sf::View& getWindowView() const;
    const sf::View& getStatView() const;

    sf::Vector2f getPositionInGame(const sf::Vector2f& positionOnWindow) const;
    sf::Vector2f getPositionOnWindow(const sf::Vector2f& positionInGame) const;
    sf::Vector2f getViewDifference() const;
};
