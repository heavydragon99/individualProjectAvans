#include "particleSystem.h"
#include "physics.h"
#include <cmath>

ParticleSystem::ParticleSystem(unsigned int aNumParticles, const sf::Vector2u &aWindowSize, float aParticleRadius, float aParticleSpacing) 
    : mWindowSize(aWindowSize), mParticleSpacing(aParticleSpacing)
{
    mPhysicsEngine = std::make_unique<Physics>(mParticles);
    spawnParticles(aNumParticles, aWindowSize, aParticleRadius);
}

void ParticleSystem::update(sf::Time aDeltaTime, const sf::Vector2u &aWindowSize)
{
    mPhysicsEngine->update(aDeltaTime, aWindowSize);
}

const std::vector<Particle> &ParticleSystem::getParticles() const
{
    return mParticles;
}

void ParticleSystem::setParticleCount(unsigned int aNumParticles)
{
    float radius = mParticles.front().mRadius;
    mParticles.clear();
    spawnParticles(aNumParticles, mWindowSize, radius);
}

void ParticleSystem::setParticleRadius(float aParticleRadius)
{
    for (auto &p : mParticles)
    {
        p.mRadius = aParticleRadius;
    }
}

void ParticleSystem::setParticleSpacing(float aParticleSpacing)
{
    mParticleSpacing = aParticleSpacing;
    float radius = mParticles.front().mRadius;
    unsigned int numParticles = mParticles.size();
    mParticles.clear();
    spawnParticles(numParticles, mWindowSize, radius);
}

void ParticleSystem::spawnParticles(unsigned int aNumParticles, const sf::Vector2u &aWindowSize, float aParticleRadius){
    // Create a grid of particles.
    int particlesPerRow = static_cast<int>(std::sqrt(aNumParticles));
    int particlesPerColumn = (aNumParticles - 1) / particlesPerRow + 1;

    // Calculate offsets to center the grid.
    float offsetX = aWindowSize.x * 0.5f;
    float offsetY = aWindowSize.y * 0.5f;

    for (unsigned int i = 0; i < aNumParticles; i++)
    {
        float x = (i % particlesPerRow - particlesPerRow * 0.5f + 0.5f) * mParticleSpacing + offsetX;
        float y = (i / particlesPerRow - particlesPerColumn * 0.5f + 0.5f) * mParticleSpacing + offsetY;
        mParticles.emplace_back(sf::Vector2f(x, y), sf::Vector2f(0.f, 0.f), aParticleRadius);
    }
}
