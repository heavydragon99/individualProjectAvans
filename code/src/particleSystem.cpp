#include "particleSystem.h"
#include "physics.h"
#include <cmath>

ParticleSystem::ParticleSystem(unsigned int aNumParticles, const sf::Vector2u &aWindowSize, float aParticleRadius)
{
    // Create a grid of particles.
    int particlesPerRow = static_cast<int>(std::sqrt(aNumParticles));
    int particlesPerColumn = (aNumParticles - 1) / particlesPerRow + 1;
    float particleSpacing = 10.f;
    float spacing = aParticleRadius * 2 + particleSpacing;

    // Calculate offsets to center the grid.
    float offsetX = aWindowSize.x * 0.5f;
    float offsetY = aWindowSize.y * 0.5f;

    for (unsigned int i = 0; i < aNumParticles; i++)
    {
        float x = (i % particlesPerRow - particlesPerRow * 0.5f + 0.5f) * spacing + offsetX;
        float y = (i / particlesPerRow - particlesPerColumn * 0.5f + 0.5f) * spacing + offsetY;
        mParticles.emplace_back(sf::Vector2f(x, y), sf::Vector2f(0.f, 0.f), aParticleRadius);
    }
}

void ParticleSystem::update(float aDeltaTime, const sf::Vector2u &aWindowSize)
{
    updateParticles(mParticles, aDeltaTime, aWindowSize);
}

const std::vector<Particle>& ParticleSystem::getParticles() const
{
    return mParticles;
}