#include "View.h"

View::View(sf::RenderWindow& window, Player* player, float scrollingSpeed)
    : windowWidth_(window.getSize().x),
      windowHeight_(window.getSize().y),
      viewDifference_(0.0f, 0.0f),
      windowView_(window.getDefaultView()),
      statView_(window.getDefaultView()),
      player_(player),
      originalPlayerSize_(player->getSize().y),
      scrollingSpeed_(scrollingSpeed) {}

void View::update(const float elapsedTime) {
    viewDifference_ = statView_.getCenter() - windowView_.getCenter();

    const sf::Vector2f playerPosOnWindow = getPositionOnWindow(player_->getPosition());

    const sf::Vector2f playerMovement = playerPosOnWindow - getPositionOnWindow(prevPlayerPos_);

    const sf::Vector2f playerSize = player_->getSize();

    sf::Vector2f newCenter = windowView_.getCenter();

    const float windowWidthFifth = windowWidth_ / 5.0f;

    const float scrollingDistance = scrollingSpeed_ * elapsedTime;

    if (playerMovement.x > 0 && playerPosOnWindow.x + playerSize.x > windowWidthFifth * 2) {  // Player is moving right
        newCenter += sf::Vector2f(scrollingDistance, 0);
    } else if (playerMovement.x < 0 && playerPosOnWindow.x < windowWidthFifth * 3) {  // Player is moving left
        newCenter -= sf::Vector2f(scrollingDistance, 0);
    }

    if (playerMovement.y > 0 && playerPosOnWindow.y + playerSize.y > windowHeight_ - originalPlayerSize_ * 2) {  // Player is moving down
        newCenter += sf::Vector2f(0, scrollingDistance);
    } else if (playerMovement.y < 0 && playerPosOnWindow.y < windowHeight_ / 5 * 3) {  // Player is moving up
        newCenter -= sf::Vector2f(0, scrollingDistance);
    }

    windowView_.setCenter(newCenter);
    prevPlayerPos_ = player_->getPosition();
}

sf::View View::getWindowView() const {
    return windowView_;
}

sf::View View::getStatView() const {
    return statView_;
}

sf::Vector2f View::getPositionInGame(const sf::Vector2f& positionOnWindow) const {
    return positionOnWindow - viewDifference_;
}

sf::Vector2f View::getPositionOnWindow(const sf::Vector2f& positionInGame) const {
    return positionInGame + viewDifference_;
}

sf::Vector2f View::getViewDifference() const {
    return viewDifference_;
}
