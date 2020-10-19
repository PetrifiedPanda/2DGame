#include "SoundManager.h"

SoundManager::SoundManager(const std::string& soundTrackFile, const std::string& collisionSoundFile, const std::string& deathSoundFile, const std::string& pauseSoundFile) {
    soundtrackBuffer_.loadFromFile(soundTrackFile);
    collisionBuffer_.loadFromFile(collisionSoundFile);
    deathBuffer_.loadFromFile(deathSoundFile);
    pauseBuffer_.loadFromFile(pauseSoundFile);

    soundtrackSound_.setBuffer(soundtrackBuffer_);
    soundtrackSound_.setVolume(g_musicVolume * (g_masterVolume / 100));
    soundtrackSound_.setPitch(1.0f);
    soundtrackSound_.setLoop(true);

    pauseSound_.setBuffer(pauseBuffer_);

    deadIDs_.reserve(soundLimit);
}

void SoundManager::playSoundTrack() {
    soundtrackSound_.play();
}

void SoundManager::pauseSoundTrack() {
    soundtrackSound_.pause();
}

void SoundManager::stopSoundTrack() {
    soundtrackSound_.stop();
}

void SoundManager::playPauseSound() {
    pauseSound_.play();
}

void SoundManager::pausePauseSound() {
    pauseSound_.pause();
}

void SoundManager::stopPauseSound() {
    pauseSound_.stop();
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
    playEntitySound(entityID, collisionBuffer_);
}

void SoundManager::playDeathSound(int entityID) {
    playEntitySound(entityID, deathBuffer_);
}

void SoundManager::playEntitySound(int entityID, sf::SoundBuffer& buffer) {
    sf::Sound& currentSound = entitySounds_[entityID];

    if (currentSound.getStatus() != sf::Sound::Playing || currentSound.getBuffer() != &buffer) {
        currentSound.stop();
        currentSound.setBuffer(buffer);
    }
    currentSound.play();
}
