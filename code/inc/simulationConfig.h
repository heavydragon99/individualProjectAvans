#ifndef SIMULATION_CONFIG_H
#define SIMULATION_CONFIG_H

#include <functional>
#include <SFML/System/Vector2.hpp>

class SimulationConfig
{
public:
    static SimulationConfig &getInstance();

    // Getters and setters for simulation parameters
    bool paused() const;
    void paused(bool paused);

    unsigned int particleCount() const;
    void particleCount(unsigned int aCount);

    float particleRadius() const;
    void particleRadius(float aRadius);

    int simulationSpeed() const;
    void simulationSpeed(int aSpeed);

    int particleSpacing() const;
    void particleSpacing(int aSpacing);

    sf::Vector2u gameSize() const;
    void gameSize(const sf::Vector2u &aSize);

    sf::Vector2u windowSize() const;
    void windowSize(const sf::Vector2u &aSize);

    // Callback setters
    void setOnParticleCountChanged(const std::function<void()> &callback);
    void setOnParticleRadiusChanged(const std::function<void()> &callback);
    void setOnParticleSpacingChanged(const std::function<void()> &callback);

private:
    SimulationConfig();
    SimulationConfig(const SimulationConfig &) = delete;
    SimulationConfig &operator=(const SimulationConfig &) = delete;

    // Simulation parameters
    bool mPaused;
    unsigned int mParticleCount;
    float mParticleRadius;
    int mSimulationSpeed;
    int mParticleSpacing;
    sf::Vector2u mGameSize;
    sf::Vector2u mWindowSize;

    // Callbacks
    std::function<void()> onParticleCountChanged;
    std::function<void()> onParticleRadiusChanged;
    std::function<void()> onParticleSpacingChanged;
};

#endif // SIMULATION_CONFIG_H
