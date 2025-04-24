#include "simulationConfig.h"

SimulationConfig &SimulationConfig::getInstance()
{
    static SimulationConfig instance;
    return instance;
}

SimulationConfig::SimulationConfig()
    : mSimulationState(SimulationState::SETUP), mParticleCount(1200), mParticleRadius(2.f), mParticleSpacing(8), mGameSize({640, 360}), mWindowSize({1920, 1080}), mSmoothingRadius(10), mTargetDensity(40), mPressureMultiplier(1), mViscosityMultiplier(0.2), mGravity(9.81), mIsMousePressedLeft(false), mIsMousePressedRight(false), mMousePosition{0.0f, 0.0f}, mUseGPU(true) {}

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

float SimulationConfig::viscosityMultiplier() const { return mViscosityMultiplier; }
void SimulationConfig::viscosityMultiplier(float aMultiplier) { mViscosityMultiplier = aMultiplier; }

float SimulationConfig::gravity() const { return mGravity; }
void SimulationConfig::gravity(float aGravity) { mGravity = aGravity; }

bool SimulationConfig::isMousePressedLeft() const { return mIsMousePressedLeft; }
void SimulationConfig::isMousePressedLeft(bool aPressed) { mIsMousePressedLeft = aPressed; }

bool SimulationConfig::isMousePressedRight() const { return mIsMousePressedRight; }
void SimulationConfig::isMousePressedRight(bool aPressed) { mIsMousePressedRight = aPressed; }

sf::Vector2f SimulationConfig::mousePosition() const { return mMousePosition; }
void SimulationConfig::mousePosition(const sf::Vector2f &aPosition) { mMousePosition = aPosition; }

bool SimulationConfig::useGPU() const { return mUseGPU; }
void SimulationConfig::useGPU(bool aUseGPU) { mUseGPU = aUseGPU; }

void SimulationConfig::setOnParticleCountChanged(const std::function<void()> &callback) { onParticleCountChanged = callback; }
void SimulationConfig::setOnParticleRadiusChanged(const std::function<void()> &callback) { onParticleRadiusChanged = callback; }
void SimulationConfig::setOnParticleSpacingChanged(const std::function<void()> &callback) { onParticleSpacingChanged = callback; }
