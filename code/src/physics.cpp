#include "physics.h"

#include "simulationConfig.h"

#include <cmath>
#include <algorithm>

extern "C"
{
    double calculatePosition(double initialPosition, double initialVelocity, double time, double acceleration)
    {
        return initialPosition + initialVelocity * time + 0.5 * acceleration * time * time;
    }
}

Physics::Physics(std::vector<Particle>& aParticles): mParticles(aParticles) {}

void Physics::update(sf::Time aDeltaTime)
{
    applyGravity(aDeltaTime);
    checkBoundary();
}

void Physics::applyGravity(sf::Time aDeltaTime)
{
    for (auto &particle : mParticles)
    {
        particle.mVelocity.y += GRAVITY * aDeltaTime.asSeconds();   // Apply gravity.
        particle.mPosition += particle.mVelocity * aDeltaTime.asSeconds(); // Update position.
    }
}

void Physics::checkBoundary()
{
    const sf::Vector2u windowSize = SimulationConfig::getInstance().windowSize();
    for (auto &particle : mParticles)
    {
        // Simple boundary collision detection.
        if (particle.mPosition.x < particle.mRadius)
        {
            particle.mPosition.x = particle.mRadius;
            particle.mVelocity.x = -particle.mVelocity.x * COLLISION_DAMPING;
        }
        else if (particle.mPosition.x > windowSize.x - particle.mRadius)
        {
            particle.mPosition.x = windowSize.x - particle.mRadius;
            particle.mVelocity.x = -particle.mVelocity.x * COLLISION_DAMPING;
        }
        if (particle.mPosition.y < particle.mRadius)
        {
            particle.mPosition.y = particle.mRadius;
            particle.mVelocity.y = -particle.mVelocity.y * COLLISION_DAMPING;
        }
        else if (particle.mPosition.y > windowSize.y - particle.mRadius)
        {
            particle.mPosition.y = windowSize.y - particle.mRadius;
            particle.mVelocity.y = -particle.mVelocity.y * COLLISION_DAMPING;
        }
    }
}

float Physics::smoothingKernel(float aRadius, float aDistance){
    float volume = M_PI * std::pow(aRadius, 8) / 4;
    float value = std::max(0.0f, aRadius * aRadius - aDistance * aDistance);
    return value * value * value / volume;
}

float Physics::calculateDensity(int aParticleIndex){
    float density = 0.0f;
    const float mass = 10.0f;

    float smoothingRadius = SimulationConfig::getInstance().smoothingRadius();
    Particle &sampleParticle = mParticles[aParticleIndex];
    for (const auto &otherParticle : mParticles)
    {
        float distance = std::hypot(otherParticle.mPosition.x - sampleParticle.mPosition.x, otherParticle.mPosition.y - sampleParticle.mPosition.y);
        float influence = smoothingKernel(smoothingRadius, distance);
        density += mass * influence;
    }
    return density;
}