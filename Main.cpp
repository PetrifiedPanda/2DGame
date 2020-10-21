#include <chrono>

#include "SFML/Graphics.hpp"

#include "Utility/Timer.h"

#include "Global.h"
#include "World.h"
#include "View.h"
#include "SplitterEnemy.h"
#include "GrowingPlayer.h"
#include "SoundManager.h"

#define EDITOR

void addFloor(World& world);
void addPlatform(World& world, const sf::Vector2f& position, float length);
void addPillar(World& world, const sf::Vector2f& position, float width, float height);
void spawnEnemy(World& world, const sf::Vector2f& position);

void handleWindowEvents(sf::RenderWindow& window, World& world, bool& pause, bool& pauseReleased, View& view, sf::Vector2f& mouseOffset, Moveable*& selectedMoveable, Moveable*& draggedMoveable);
void handleWindowResize(sf::Event& event, sf::RenderWindow& window, World& world, View& view);
void handleMouseEvents(sf::Event& event, World& world, sf::RenderWindow& window, View& view, sf::Vector2f& mouseOffset, Moveable*& selectedMoveable, Moveable*& draggedMoveable);
void handleKeyboardEvents(sf::Event& event, World& world, bool& pause, bool& pauseReleased);

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

sf::Vector2f scale(static_cast<float>(windowWidth) / referenceWidth, static_cast<float>(windowHeight) / referenceHeight);

// World Properties (These should be in relation to window dimensions)

float groundWidth = 40.0f * scale.y;

float platformWidth = 10.0f * scale.y;

sf::Vector2f playerSize(10.0f * scale.x, 10.0f * scale.y);

int main() {
    World world(100.0f * scale.x, 200.0f * scale.y);

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

    world.soundManager.playSoundTrack();

    world.addPlayer(std::make_unique<GrowingPlayer>(sf::Vector2f(0, static_cast<float>(windowHeight) - 30.0f * scale.y), playerSize, Colors().playerColor));
    View view(window, world.getPlayer(), 200.0f * scale.x);

    // Add floor and a few platforms
    addPillar(world, sf::Vector2f(60, referenceHeight - 50), 20, 50);

    int x = 500;
    int y = static_cast<int>(referenceHeight) - 50;
    while (static_cast<float>(x) < referenceWidth && y > 0) {
        addPlatform(world, sf::Vector2f(x, y), 100);
        x += 100;
        y -= 50;
    }
    x -= 100;
    y += 50;

    addFloor(world);

    auto time1 = std::chrono::system_clock::now();
    auto time2 = std::chrono::system_clock::now();

    auto fpsTime = std::chrono::system_clock::now();

    sf::Vector2f mouseOffset;
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
            draggedMoveable->setPosition(view.getPositionInGame(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)) + mouseOffset));
#endif

        if (frames == updateFrames) {
// Update stats
#ifdef EDITOR
            worldStats =
                "Cores: " + std::to_string(omp_get_max_threads()) + "\n" +
                "Moveables: " + std::to_string(world.getMoveableCount()) + "\n" +
                "Static Rectangles: " + std::to_string(world.getStaticRectangleCount()) + "\n";

            if (selectedMoveable != nullptr)
                statString = selectedMoveable->toString();
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

        handleWindowEvents(window, world, pause, pauseReleased, view, mouseOffset, selectedMoveable, draggedMoveable);

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
    floor.setSize(sf::Vector2f(windowWidth * 2, groundWidth));
    floor.setPosition(sf::Vector2f(-windowWidth, windowHeight));

    world.addRectangle(std::move(floor));
}

void addPlatform(World& world, const sf::Vector2f& position, const float length) {
    const Colors colors;

    sf::RectangleShape platform;
    platform.setFillColor(colors.environmentColor);
    platform.setSize(sf::Vector2f(length * scale.x, platformWidth));
    platform.setPosition(sf::Vector2f(position.x * scale.x, position.y * scale.y));

    world.addRectangle(std::move(platform));
}

void addPillar(World& world, const sf::Vector2f& position, const float width, const float height) {
    const Colors colors;

    sf::RectangleShape pillar;
    pillar.setFillColor(colors.environmentColor);
    pillar.setSize(sf::Vector2f(width * scale.x, height * scale.y));
    pillar.setPosition(sf::Vector2f(position.x * scale.x, position.y * scale.y));

    world.addRectangle(std::move(pillar));
}

void spawnEnemy(World& world, const sf::Vector2f& position) {
    const Colors colors;
    sf::Vector2f size(playerSize.x * 4.0f, playerSize.y * 4.0f);
    Enemy enemy(position, size, colors.enemyColor);

    if (world.canMoveInDirection(&enemy, sf::Vector2f(0, 0)))
        world.addMoveable(std::make_unique<SplitterEnemy>(position, size, colors.enemyColor));
}

void handleWindowResize(sf::Event& event, sf::RenderWindow& window, World& world, View& view) {
    windowWidth = static_cast<int>(event.size.width);
    windowHeight = static_cast<int>(event.size.height);

    sf::Vector2f oldScale(scale);

    // Recalculate scale factors
    scale = sf::Vector2f(static_cast<float>(windowWidth) / referenceWidth, static_cast<float>(windowHeight) / referenceHeight);

    sf::Vector2f diffScale(scale.x / oldScale.x, scale.y / oldScale.y);  // Scale for entities created with old scale

    // Reassign global properties
    groundWidth *= diffScale.y;
    playerSize = sf::Vector2f(playerSize.x * diffScale.x, playerSize.y * diffScale.y);

    world.scale(diffScale);
    view.windowResize(window, diffScale);
}

void handleWindowEvents(sf::RenderWindow& window, World& world, bool& pause, bool& pauseReleased, View& view, sf::Vector2f& mouseOffset, Moveable*& selectedMoveable, Moveable*& draggedMoveable) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();

        if (event.type == sf::Event::Resized)
            handleWindowResize(event, window, world, view);
#ifdef EDITOR
        handleMouseEvents(event, world, window, view, mouseOffset, selectedMoveable, draggedMoveable);
#endif

        handleKeyboardEvents(event, world, pause, pauseReleased);
    }
}

#ifdef EDITOR
void handleMouseEvents(sf::Event& event, World& world, sf::RenderWindow& window, View& view, sf::Vector2f& mouseOffset, Moveable*& selectedMoveable, Moveable*& draggedMoveable) {
    const sf::Vector2f mousePosition = view.getPositionInGame(static_cast<sf::Vector2f>(sf::Mouse::getPosition(window)));

    if (event.type == sf::Event::MouseButtonPressed) {
        if (event.key.code == sf::Mouse::Left) {
            Moveable* moveable = world.isOnMoveable(mousePosition);

            if (moveable != nullptr) {
                mouseOffset = moveable->getPosition() - mousePosition;
                draggedMoveable = selectedMoveable = moveable;
            }

        } else if (event.key.code == sf::Mouse::Right) {
            spawnEnemy(world, mousePosition);
        }
    }

    if (event.type == sf::Event::MouseButtonReleased && event.key.code == sf::Mouse::Left) {
        draggedMoveable = nullptr;
    }
}
#endif

void handleKeyboardEvents(sf::Event& event, World& world, bool& pause, bool& pauseReleased) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape) {
        if (pause) {
            if (pauseReleased) {
                pause = false;

                world.soundManager.stopPauseSound();
                world.soundManager.playSoundTrack();
            }
        } else {
            pause = true;
            pauseReleased = false;

            world.soundManager.pauseSoundTrack();
            world.soundManager.playPauseSound();
        }
    }

    if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Key::Escape) {
        pauseReleased = true;
    }
}
