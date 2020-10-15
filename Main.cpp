#include <chrono>

#include "Enemy.h"
#include "Global.h"
#include "GrowingPlayer.h"
#include "Player.h"
#include "SFML/Graphics.hpp"
#include "SoundManager.h"
#include "Utility/Timer.h"
#include "View.h"
#include "World.h"

#define EDITOR

using sf::Vector2f;

void addFloor(World& world);
void addPlatform(World& world, const Vector2f& position, float length);
void addPillar(World& world, const Vector2f& position, float width, float height);
void spawnEnemy(World& world, const Vector2f& position, SoundManager& soundManager);

void handleWindowEvents(sf::RenderWindow& window, World& world, bool& pause, bool& pauseReleased, View& view, Vector2f& mouseOffset, Moveable*& selectedMoveable, Moveable*& draggedMoveable, SoundManager& soundManager);
void handleWindowResize(sf::Event& event);
void handleMouseEvents(sf::Event& event, World& world, sf::RenderWindow& window, View& view, Vector2f& mouseOffset, Moveable*& selectedMoveable, Moveable*& draggedMoveable, SoundManager& soundManager);
void handleKeyboardEvents(sf::Event& event, bool& pause, bool& pauseReleased);

struct Colors {
    sf::Color playerColor = sf::Color::Blue;
    sf::Color enemyColor = sf::Color::Red;
    sf::Color environmentColor = sf::Color::Cyan;
    sf::Color backgroundColor = sf::Color::Black;
};

// Window Properties

int windowWidth = 1280;
int windowHeight = 720;

constexpr float referenceWidth = 1280.0f;
constexpr float referenceHeight = 720.0f;

float scaleFactorX = static_cast<float>(windowWidth) / referenceWidth;
float scaleFactorY = static_cast<float>(windowHeight) / referenceHeight;

// World Properties (These should be in relation to window dimensions)

float movementSpeed = 100.0f * scaleFactorX;

float gravity = 200.0f * scaleFactorY;

float groundWidth = 40.0f * scaleFactorY;

float platformWidth = 10.0f * scaleFactorY;

float playerSizeX = 10.0f * scaleFactorX;

float playerSizeY = 10.0f * scaleFactorY;

int main() {
    World world(movementSpeed, gravity);

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "My stupid Game");

    std::string fpsString = "FPS: \n";
    std::string worldStats;
    std::string statString;

    sf::Text stats;
    stats.setString(fpsString + statString);

    sf::Font font;
    font.loadFromFile("resources/font/Font.ttf");

    stats.setFont(font);
    stats.setCharacterSize(12);
    stats.setFillColor(sf::Color::Green);

    SoundManager soundManager("resources/sound/BackgroundMusic.wav", "resources/sound/Collision.wav", "resources/sound/Death.wav");

    soundManager.playSoundTrack();

    world.addPlayer(std::make_unique<GrowingPlayer>(Vector2f(0, static_cast<float>(windowHeight) - 30.0f * scaleFactorY), playerSizeX, playerSizeY, Colors().playerColor, soundManager));
    View view(window, dynamic_cast<Player*>(world.getPlayer()), movementSpeed * 2.0f);

    // Add floor and a few platforms
    addPillar(world, Vector2f(60, referenceHeight - 50), 20, 50);

    int x = 500;
    int y = static_cast<int>(referenceHeight) - 50;
    while (static_cast<float>(x) < referenceWidth && y > 0) {
        addPlatform(world, Vector2f(x, y), 100);
        x += 100;
        y -= 50;
    }
    x -= 100;
    y += 50;

    addFloor(world);

    auto time1 = std::chrono::system_clock::now();
    auto time2 = std::chrono::system_clock::now();

    auto fpsTime = std::chrono::system_clock::now();

    Vector2f mouseOffset;
    Moveable* selectedMoveable = nullptr;
    Moveable* draggedMoveable = nullptr;

    float fps = 0.0f;
    short frames = 0;
    short updateFrames = 100;

    bool pause = false;
    bool pauseReleased = false;
    while (window.isOpen()) {
        frames++;
        // Calculate elapsed time between this frame and the last
        time2 = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsed = time2 - time1;
        time1 = time2;

        float elapsedTime = elapsed.count();
#ifdef EDITOR
        if (draggedMoveable != nullptr && !world.containsMoveable(draggedMoveable))
            draggedMoveable = nullptr;

        if (selectedMoveable != nullptr && !world.containsMoveable(selectedMoveable))
            selectedMoveable = nullptr;

        if (draggedMoveable != nullptr)
            draggedMoveable->setPosition(view.getPositionInGame(static_cast<Vector2f>(sf::Mouse::getPosition(window)) + mouseOffset));
#endif

        if (frames == updateFrames) {
// Update stats
#ifdef EDITOR
            worldStats =
                "Moveables: " + std::to_string(world.getMoveableCount()) + "\n" +
                "Static Rectangles: " + std::to_string(world.getStaticRectangleCount()) + "\n";

            if (selectedMoveable != nullptr)
                statString = selectedMoveable->toString();  // Throws an error when selected moveable dies
#endif

            // Calculate FPS
            std::chrono::duration<float> frameDuration = time2 - fpsTime;
            float frameTime = frameDuration.count();
            fps = static_cast<float>(updateFrames) / frameTime;

            stats.setString("FPS: " + std::to_string(fps) + "\n" + worldStats + statString);

            // Reset frame counter and timer
            frames = 0;
            fpsTime = std::chrono::system_clock::now();
        }

        handleWindowEvents(window, world, pause, pauseReleased, view, mouseOffset, selectedMoveable, draggedMoveable, soundManager);

        if (!pause) {
            world.update(elapsedTime, window, draggedMoveable);
            view.update(elapsedTime);
        }

        window.clear(Colors().backgroundColor);

        window.setView(view.getWindowView());
        world.draw(window);

        window.setView(view.getStatView());
        window.draw(stats);

        window.display();
    }

    return 0;
}

void addFloor(World& world) {
    const Colors colors;

    sf::RectangleShape floor;

    floor.setFillColor(colors.environmentColor);
    floor.setSize(Vector2f(windowWidth * 2, groundWidth));
    floor.setPosition(Vector2f(-windowWidth, windowHeight));

    world.addRectangle(floor);
}

void addPlatform(World& world, const Vector2f& position, const float length) {
    const Colors colors;

    sf::RectangleShape platform;
    platform.setFillColor(colors.environmentColor);
    platform.setSize(Vector2f(length * scaleFactorX, platformWidth));
    platform.setPosition(Vector2f(position.x * scaleFactorX, position.y * scaleFactorY));

    world.addRectangle(platform);
}

void addPillar(World& world, const Vector2f& position, const float width, const float height) {
    const Colors colors;

    sf::RectangleShape pillar;
    pillar.setFillColor(colors.environmentColor);
    pillar.setSize(Vector2f(width * scaleFactorX, height * scaleFactorY));
    pillar.setPosition(Vector2f(position.x * scaleFactorX, position.y * scaleFactorY));

    world.addRectangle(pillar);
}

void spawnEnemy(World& world, const Vector2f& position, SoundManager& soundManager) {
    const Colors colors;

    Enemy enemy(position, playerSizeX, playerSizeY, colors.enemyColor, soundManager);

    if (world.canMoveInDirection(&enemy, Vector2f(0, 0)))
        world.addMoveable(std::make_unique<Enemy>(position, playerSizeX, playerSizeY, colors.enemyColor, soundManager));
}

void handleWindowResize(sf::Event& event) {
    windowWidth = static_cast<int>(event.size.width);
    windowHeight = static_cast<int>(event.size.height);

    float oldScaleFactorX = scaleFactorX;
    float oldScaleFactorY = scaleFactorY;

    // Recalculate scale factors
    scaleFactorX = static_cast<float>(windowWidth) / referenceWidth;
    scaleFactorY = static_cast<float>(windowHeight) / referenceHeight;

    // Reassign world properties
    movementSpeed = 100.0f * scaleFactorX;
    gravity = 200.0f * scaleFactorY;
    groundWidth = 10.0f * scaleFactorY;
    playerSizeX = 10.0f * scaleFactorX;
    playerSizeY = 10.0f * scaleFactorY;

    /*
	TODO:
	Adjust dimensions (and maybe positions) of all RectangleShapes and Moveables in World
	*/
}

void handleWindowEvents(sf::RenderWindow& window, World& world, bool& pause, bool& pauseReleased, View& view, Vector2f& mouseOffset, Moveable*& selectedMoveable, Moveable*& draggedMoveable, SoundManager& soundManager) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::Resized)
            handleWindowResize(event);
#ifdef EDITOR
        handleMouseEvents(event, world, window, view, mouseOffset, selectedMoveable, draggedMoveable, soundManager);
#endif

        handleKeyboardEvents(event, pause, pauseReleased);
    }
}

#ifdef EDITOR
void handleMouseEvents(sf::Event& event, World& world, sf::RenderWindow& window, View& view, Vector2f& mouseOffset, Moveable*& selectedMoveable, Moveable*& draggedMoveable, SoundManager& soundManager) {
    const Vector2f mousePosition = view.getPositionInGame(static_cast<Vector2f>(sf::Mouse::getPosition(window)));

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.key.code == sf::Mouse::Left) {
            Moveable* moveable = world.isOnMoveable(mousePosition);

            if (moveable != nullptr) {
                mouseOffset = moveable->getPosition() - mousePosition;
                draggedMoveable = selectedMoveable = moveable;
            }

        } else if (event.key.code == sf::Mouse::Right) {
            spawnEnemy(world, mousePosition, soundManager);
        }
    }

    if (event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Left) {
        draggedMoveable = nullptr;
    }
}
#endif

void handleKeyboardEvents(sf::Event& event, bool& pause, bool& pauseReleased) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape) {
        if (pause) {
            if (pauseReleased)
                pause = false;
        } else {
            pause = true;
            pauseReleased = false;
        }
    }

    if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Key::Escape) {
        pauseReleased = true;
    }
}
