#pragma once

#include "SFML/Audio.hpp"

#include "Global.h"

class SoundManager {
    sf::SoundBuffer soundtrackBuffer_;

    sf::SoundBuffer collisionBuffer_;
    sf::SoundBuffer deathBuffer_;

    sf::SoundBuffer pauseBuffer_;

    sf::Sound soundtrackSound_;
    sf::Sound pauseSound_;
    std::unordered_map<int, sf::Sound> entitySounds_;

    std::vector<int> deadIDs_;
    static const int soundLimit = 200;

   public:
    SoundManager(const std::string& soundTrackFile, const std::string& collisionSoundFile, const std::string& deathSoundFile, const std::string& pauseSoundFile);

    void playSoundTrack();
    void pauseSoundTrack();
    void stopSoundTrack();

    void playPauseSound();
    void pausePauseSound();
    void stopPauseSound();

    void addEntitySound(int entityID);
    void removeEntity(int entityID);

    void playCollisionSound(int entityID);
    void playDeathSound(int entityID);

   private:
    void playEntitySound(int entityID, sf::SoundBuffer& buffer);
};
