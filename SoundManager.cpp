#include "SoundManager.h"

SoundManager::SoundManager(const std::string& soundTrackFile, const std::string& collisionSoundFile, const std::string& deathSoundFile) {
    soundtrackBuffer_.loadFromFile(soundTrackFile);
    collisionSoundBuffer_.loadFromFile(collisionSoundFile);
    deathSoundBuffer_.loadFromFile(deathSoundFile);

    soundTrackSound_.setBuffer(soundtrackBuffer_);
    soundTrackSound_.setVolume(g_musicVolume * (g_masterVolume / 100));
    soundTrackSound_.setPitch(1.0f);
    soundTrackSound_.setLoop(true);

    deadIDs_.reserve(soundLimit);
}

void SoundManager::playSoundTrack() {
    soundTrackSound_.play();
}

void SoundManager::pauseSoundTrack() {
    soundTrackSound_.pause();
}

void SoundManager::stopSoundTrack() {
    soundTrackSound_.stop();
}

void SoundManager::addEntitySound(int entityID) {
    if (entitySounds_.size() == soundLimit) {
        entitySounds_.erase(deadIDs_[0]);
        deadIDs_.erase(deadIDs_.begin());
    }

    entitySounds_.insert(std::pair<int, sf::Sound>(entityID, sf::Sound()));
    entitySounds_[entityID].setVolume(g_effectVolume * (g_masterVolume / 100));
}

void SoundManager::removeEntity(int entityID) {
    deadIDs_.push_back(entityID);
}

void SoundManager::playCollisionSound(int entityID) {
    playEntitySound(entityID, collisionSoundBuffer_);
}

void SoundManager::playDeathSound(int entityID) {
    playEntitySound(entityID, deathSoundBuffer_);
}

void SoundManager::playEntitySound(int entityID, sf::SoundBuffer& buffer) {
    sf::Sound& currentSound = entitySounds_[entityID];

    if (currentSound.getStatus() != sf::Sound::Playing || currentSound.getBuffer() != &buffer) {
        currentSound.stop();
        currentSound.setBuffer(buffer);
    }
    currentSound.play();
}
