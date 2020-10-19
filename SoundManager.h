#pragma once

#include "SFML/Audio.hpp"

#include "Global.h"

class SoundManager {
    sf::SoundBuffer soundtrackBuffer_;

    sf::SoundBuffer collisionSoundBuffer_;
    sf::SoundBuffer deathSoundBuffer_;

    sf::Sound soundTrackSound_;
    std::unordered_map<int, sf::Sound> entitySounds_;

    std::vector<int> deadIDs_;
    static const int soundLimit = 200;

   public:
    SoundManager(const std::string& soundTrackFile, const std::string& collisionSoundFile, const std::string& deathSoundFile);

    void playSoundTrack();
    void pauseSoundTrack();
    void stopSoundTrack();

    void addEntitySound(int entityID);
    void removeEntity(int entityID);

    void playCollisionSound(int entityID);
    void playDeathSound(int entityID);

   private:
    void playEntitySound(int entityID, sf::SoundBuffer& buffer);
};
