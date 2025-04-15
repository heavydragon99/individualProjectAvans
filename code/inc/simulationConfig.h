#ifndef SIMULATION_CONFIG_H
#define SIMULATION_CONFIG_H

#include <functional>
#include <SFML/System/Vector2.hpp>

enum class SimulationState
{
    SETUP,
    INITIALIZING,
    RUNNING,
    PAUSED,
    RESET
};

class SimulationConfig
{
public:
    static SimulationConfig &getInstance();

    // Getters and setters for simulation parameters
    SimulationState simulationState() const;
    void simulationState(SimulationState aState);

    unsigned int particleCount() const;
    void particleCount(unsigned int aCount);

    float particleRadius() const;
    void particleRadius(float aRadius);

    int particleSpacing() const;
    void particleSpacing(int aSpacing);

    sf::Vector2u gameSize() const;
    void gameSize(const sf::Vector2u &aSize);

    sf::Vector2u windowSize() const;
    void windowSize(const sf::Vector2u &aSize);

    float smoothingRadius() const;
    void smoothingRadius(float aRadius);

    float targetDensity() const;
    void targetDensity(float aDensity);

    float pressureMultiplier() const;
    void pressureMultiplier(float aMultiplier);

    float viscosityMultiplier() const;
    void viscosityMultiplier(float aMultiplier);

    float gravity() const;
    void gravity(float aGravity);

    bool isMousePressedLeft() const;
    void isMousePressedLeft(bool aPressed);

    bool isMousePressedRight() const;
    void isMousePressedRight(bool aPressed);

    sf::Vector2f mousePosition() const;
    void mousePosition(const sf::Vector2f &aPosition);

    // Callback setters
    void setOnParticleCountChanged(const std::function<void()> &callback);
    void setOnParticleRadiusChanged(const std::function<void()> &callback);
    void setOnParticleSpacingChanged(const std::function<void()> &callback);

private:
    SimulationConfig();
    SimulationConfig(const SimulationConfig &) = delete;
    SimulationConfig &operator=(const SimulationConfig &) = delete;

    // Simulation parameters
    SimulationState mSimulationState;
    unsigned int mParticleCount;
    float mParticleRadius;
    int mParticleSpacing;
    sf::Vector2u mGameSize;
    sf::Vector2u mWindowSize;
    float mSmoothingRadius;
    float mTargetDensity;
    float mPressureMultiplier;
    float mViscosityMultiplier;
    float mGravity;
    bool mIsMousePressedLeft;
    bool mIsMousePressedRight;
    sf::Vector2f mMousePosition;

    // Callbacks
    std::function<void()>
        onParticleCountChanged;
    std::function<void()> onParticleRadiusChanged;
    std::function<void()> onParticleSpacingChanged;
};

#endif // SIMULATION_CONFIG_H
