#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "particle.h"

class ParticleSystem
{
public:
    ParticleSystem(unsigned int aNumParticles, const sf::Vector2u &aWindowSize, float aParticleRadius);

    void update(float aDeltaTime, float aGravity, float aCollisionDamping, const sf::Vector2u &aWindowSize);
    void draw(sf::RenderTarget &aTarget);

private:
    std::vector<Particle> mParticles;
    float mParticleRadius;
};

#endif // PARTICLE_SYSTEM_H