#include "SplitterEnemy.h"

SplitterEnemy::SplitterEnemy() : Enemy() {}
SplitterEnemy::SplitterEnemy(const sf::Vector2f& position, const sf::Vector2f& size, sf::Color enemyColor)
    : Enemy(position, size, enemyColor) {}

void SplitterEnemy::onDeath(World& world) {
    Enemy enemy1(sf::Vector2f(getPosition().x - getSize().x, getPosition().y + getSize().y / 2.0f),
                 sf::Vector2f(getSize().x / 2.0f,
                              getSize().y / 2.0f),
                 getFillColor());
    const sf::Vector2f zeroVec(0, 0);
    if (world.canMoveInDirection(&enemy1, zeroVec))
        world.addMoveable(std::make_unique<Enemy>(sf::Vector2f(getPosition().x - getSize().x, getPosition().y + getSize().y / 2.0f),
                                                  sf::Vector2f(getSize().x / 2.0f,
                                                               getSize().y / 2.0f),
                                                  getFillColor()));
    Enemy enemy2 = Enemy(sf::Vector2f(getPosition().x + getSize().x * 2.0f, getPosition().y + getSize().y / 2.0f),
                         sf::Vector2f(getSize().x / 2.0f,
                                      getSize().y / 2.0f),
                         getFillColor());
    if (world.canMoveInDirection(&enemy2, zeroVec))
        world.addMoveable(std::make_unique<Enemy>(sf::Vector2f(getPosition().x + getSize().x * 2.0f, getPosition().y + getSize().y / 2.0f),
                                                  sf::Vector2f(getSize().x / 2.0f,
                                                               getSize().y / 2.0f),
                                                  getFillColor()));
}