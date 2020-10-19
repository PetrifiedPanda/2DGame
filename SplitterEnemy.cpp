#include "SplitterEnemy.h"

SplitterEnemy::SplitterEnemy(int generationNum) : Enemy(), generationNum_(generationNum) {}

SplitterEnemy::SplitterEnemy(const sf::Vector2f& position, const sf::Vector2f& size, sf::Color enemyColor, int generationNum)
    : Enemy(position, size, enemyColor), generationNum_(generationNum) {}

void SplitterEnemy::onDeath(World& world) {
    const sf::Vector2f enemy1Pos(getPosition().x - getSize().x, getPosition().y + getSize().y / 2.0f);
    const sf::Vector2f enemy2Pos(getPosition().x + getSize().x * 2.0f, getPosition().y + getSize().y / 2.0f);

    const sf::Vector2f enemySize(getSize().x / 2.0f, getSize().y / 2.0f);
    const sf::Vector2f zeroVec(0, 0);

    if (generationNum_ == s_numChildSpawns - 1) {  // This generation will be the last that spawns other Enemies
        Enemy enemy1(enemy1Pos, enemySize, getFillColor());

        if (world.canMoveInDirection(&enemy1, zeroVec))
            world.addMoveable(std::make_unique<Enemy>(enemy1Pos, enemySize, getFillColor()));

        Enemy enemy2(enemy2Pos, enemySize, getFillColor());
        if (world.canMoveInDirection(&enemy2, zeroVec))
            world.addMoveable(std::make_unique<Enemy>(enemy2Pos, enemySize, getFillColor()));
    } else {
        const int nextGenNum = generationNum_ + 1;

        SplitterEnemy enemy1(enemy1Pos, enemySize, getFillColor(), nextGenNum);
        if (world.canMoveInDirection(&enemy1, zeroVec))
            world.addMoveable(std::make_unique<SplitterEnemy>(enemy1Pos, enemySize, getFillColor(), nextGenNum));

        SplitterEnemy enemy2 = SplitterEnemy(enemy2Pos, enemySize, getFillColor(), nextGenNum);
        if (world.canMoveInDirection(&enemy2, zeroVec))
            world.addMoveable(std::make_unique<SplitterEnemy>(enemy2Pos, enemySize, getFillColor(), nextGenNum));
    }
}

MoveableType SplitterEnemy::getType() const {
    return MoveableType::SPLITTERENEMY;
}

std::string SplitterEnemy::toString() const {
    return Enemy::toString() +
           "generationNum = " + std::to_string(generationNum_);
}