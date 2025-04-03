#include "simulationConfig.h"

SimulationConfig &SimulationConfig::getInstance()
{
    static SimulationConfig instance;
    return instance;
}

SimulationConfig::SimulationConfig()
    : mSimulationState(SimulationState::SETUP), mParticleCount(1000), mParticleRadius(8.f), mSimulationSpeed(1), mParticleSpacing(30), mGameSize({1920, 1080}), mWindowSize({1920, 1080}), mSmoothingRadius(50), mTargetDensity(10), mPressureMultiplier(100) {}

SimulationState SimulationConfig::simulationState() const { return mSimulationState; }
void SimulationConfig::simulationState(SimulationState aState) { mSimulationState = aState; }

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

float SimulationConfig::smoothingRadius() const { return mSmoothingRadius; }
void SimulationConfig::smoothingRadius(float aRadius) { mSmoothingRadius = aRadius; }

float SimulationConfig::targetDensity() const { return mTargetDensity; }
void SimulationConfig::targetDensity(float aDensity) { mTargetDensity = aDensity; }

float SimulationConfig::pressureMultiplier() const { return mPressureMultiplier; }
void SimulationConfig::pressureMultiplier(float aMultiplier) { mPressureMultiplier = aMultiplier; }

void SimulationConfig::setOnParticleCountChanged(const std::function<void()> &callback) { onParticleCountChanged = callback; }
void SimulationConfig::setOnParticleRadiusChanged(const std::function<void()> &callback) { onParticleRadiusChanged = callback; }
void SimulationConfig::setOnParticleSpacingChanged(const std::function<void()> &callback) { onParticleSpacingChanged = callback; }
