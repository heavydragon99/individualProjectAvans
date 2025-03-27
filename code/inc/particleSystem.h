#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include <SFML/System/Clock.hpp>
#include <memory>

#include "particle.h"
#include "physics.h"

class ParticleSystem
{
public:
    ParticleSystem(unsigned int aNumParticles, const sf::Vector2u &aWindowSize, float aParticleRadius, float aParticleSpacing);

    void update(sf::Time aDeltaTime, const sf::Vector2u &aWindowSize);
    const std::vector<Particle>& getParticles() const;
    void setParticleCount(unsigned int aNumParticles);
    void setParticleRadius(float aParticleRadius);
    void setParticleSpacing(float aParticleSpacing);

private:
    void spawnParticles(unsigned int aNumParticles, const sf::Vector2u &aWindowSize, float aParticleRadius);

    std::vector<Particle> mParticles;
    sf::Vector2u mWindowSize;
    float mParticleSpacing; // New member variable for particle spacing

    std::unique_ptr<Physics> mPhysicsEngine;
};

#endif // PARTICLE_SYSTEM_H