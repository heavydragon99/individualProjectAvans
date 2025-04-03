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
    ParticleSystem();

    void update(sf::Time aDeltaTime);
    void reset();
    const std::vector<Particle>& getParticles() const;
    void updatedParticleCount();
    void updatedParticleRadius();
    void updatedParticleSpacing();

private:
    void spawnParticles();

    std::vector<Particle> mParticles;
    std::unique_ptr<Physics> mPhysicsEngine;
};

#endif // PARTICLE_SYSTEM_H