#include "simulationConfig.h"

SimulationConfig &SimulationConfig::getInstance()
{
    static SimulationConfig instance;
    return instance;
}

SimulationConfig::SimulationConfig()
    : mPaused(true), mParticleCount(100), mParticleRadius(5.f), mSimulationSpeed(5), mParticleSpacing(15), mGameSize({1920, 1080}), mWindowSize({1920, 1080}) {}

bool SimulationConfig::paused() const { return mPaused; }
void SimulationConfig::paused(bool paused) { mPaused = paused; }

unsigned int SimulationConfig::particleCount() const { return mParticleCount; }
void SimulationConfig::particleCount(unsigned int aCount)
{
    mParticleCount = aCount;
    if (onParticleCountChanged)
    {
        onParticleCountChanged();
    }
}

float SimulationConfig::particleRadius() const { return mParticleRadius; }
void SimulationConfig::particleRadius(float aRadius)
{
    mParticleRadius = aRadius;
    if (onParticleRadiusChanged)
    {
        onParticleRadiusChanged();
    }
}

int SimulationConfig::simulationSpeed() const { return mSimulationSpeed; }
void SimulationConfig::simulationSpeed(int aSpeed) { mSimulationSpeed = aSpeed; }

int SimulationConfig::particleSpacing() const { return mParticleSpacing; }
void SimulationConfig::particleSpacing(int aSpacing)
{
    mParticleSpacing = aSpacing;
    if (onParticleSpacingChanged)
    {
        onParticleSpacingChanged();
    }
}

sf::Vector2u SimulationConfig::gameSize() const { return mGameSize; }
void SimulationConfig::gameSize(const sf::Vector2u &aSize) { mGameSize = aSize; }

sf::Vector2u SimulationConfig::windowSize() const { return mWindowSize; }
void SimulationConfig::windowSize(const sf::Vector2u &aSize) { mWindowSize = aSize; }

void SimulationConfig::setOnParticleCountChanged(const std::function<void()> &callback) { onParticleCountChanged = callback; }
void SimulationConfig::setOnParticleRadiusChanged(const std::function<void()> &callback) { onParticleRadiusChanged = callback; }
void SimulationConfig::setOnParticleSpacingChanged(const std::function<void()> &callback) { onParticleSpacingChanged = callback; }
