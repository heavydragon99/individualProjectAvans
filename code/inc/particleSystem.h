#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <vector>
#include <SFML/System/Clock.hpp>
#include "particle.h"

class ParticleSystem
{
public:
    ParticleSystem(unsigned int aNumParticles, const sf::Vector2u &aWindowSize, float aParticleRadius);

    void update(sf::Time aDeltaTime, const sf::Vector2u &aWindowSize);
    const std::vector<Particle>& getParticles() const;

private:
    std::vector<Particle> mParticles;
};

#endif // PARTICLE_SYSTEM_H