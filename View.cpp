#include "View.h"

View::View(sf::RenderWindow& window, Moveable* followed, float scrollingSpeed)
    : windowWidth_(window.getSize().x),
      windowHeight_(window.getSize().y),
      viewDifference_(0.0f, 0.0f),
      windowView_(window.getDefaultView()),
      statView_(window.getDefaultView()),
      followed_(followed),
      originalPlayerSize_(followed->getSize()),
      scrollingSpeed_(scrollingSpeed) {}

void View::update(const float elapsedTime) {
    viewDifference_ = statView_.getCenter() - windowView_.getCenter();

    const sf::Vector2f followedPosOnWindow = getPositionOnWindow(followed_->getPosition());

    const sf::Vector2f followedMovement = followedPosOnWindow - getPositionOnWindow(prevPlayerPos_);

    const sf::Vector2f followedSize = followed_->getSize();

    sf::Vector2f newCenter = windowView_.getCenter();

    const float windowWidthFifth = windowWidth_ / 5.0f;

    const float scrollingDistance = scrollingSpeed_ * elapsedTime;

    if (followedMovement.x > 0 && followedPosOnWindow.x + followedSize.x > windowWidthFifth * 2) {  // Player is moving right
        newCenter += sf::Vector2f(scrollingDistance, 0);
    } else if (followedMovement.x < 0 && followedPosOnWindow.x < windowWidthFifth * 3) {  // Player is moving left
        newCenter -= sf::Vector2f(scrollingDistance, 0);
    }

    if (followedMovement.y > 0 && followedPosOnWindow.y + followedSize.y > windowHeight_ - originalPlayerSize_.y * 2) {  // Player is moving down
        newCenter += sf::Vector2f(0, scrollingDistance);
    } else if (followedMovement.y < 0 && followedPosOnWindow.y < windowHeight_ / 5 * 3) {  // Player is moving up
        newCenter -= sf::Vector2f(0, scrollingDistance);
    }

    windowView_.setCenter(newCenter);
    prevPlayerPos_ = followed_->getPosition();
}

void View::windowResize(sf::RenderWindow& window, sf::Vector2f scale) {
    windowWidth_ = window.getSize().x;
    windowHeight_ = window.getSize().y;

    sf::Vector2f center(windowView_.getCenter().x * scale.x, windowView_.getCenter().y * scale.y);
    sf::FloatRect visibleArea(0, 0, window.getSize().x, window.getSize().y);  // Change visible area to new window size
    windowView_ = sf::View(visibleArea);
    windowView_.setCenter(center);
    statView_ = sf::View(visibleArea);
    viewDifference_ = statView_.getCenter() - windowView_.getCenter();

    prevPlayerPos_ = sf::Vector2f(prevPlayerPos_.x * scale.x,
                                  prevPlayerPos_.y * scale.y);

    scrollingSpeed_ *= scale.x;

    originalPlayerSize_ = sf::Vector2f(originalPlayerSize_.x * scale.x, originalPlayerSize_.y * scale.y);
}

const sf::View& View::getWindowView() const {
    return windowView_;
}

const sf::View& View::getStatView() const {
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