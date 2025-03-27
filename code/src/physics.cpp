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
    updateParticles(aDeltaTime);
}

void Physics::updateParticles(sf::Time aDeltaTime)
{
    const sf::Vector2u windowSize = SimulationConfig::getInstance().windowSize();
    for (auto &p : mParticles)
    {
        p.mVelocity.y += GRAVITY * aDeltaTime.asSeconds();   // Apply gravity.
        p.mPosition += p.mVelocity * aDeltaTime.asSeconds(); // Update position.

        // Simple boundary collision detection.
        if (p.mPosition.x < p.mRadius)
        {
            p.mPosition.x = p.mRadius;
            p.mVelocity.x = -p.mVelocity.x * COLLISION_DAMPING;
        }
        else if (p.mPosition.x > windowSize.x - p.mRadius)
        {
            p.mPosition.x = windowSize.x - p.mRadius;
            p.mVelocity.x = -p.mVelocity.x * COLLISION_DAMPING;
        }
        if (p.mPosition.y < p.mRadius)
        {
            p.mPosition.y = p.mRadius;
            p.mVelocity.y = -p.mVelocity.y * COLLISION_DAMPING;
        }
        else if (p.mPosition.y > windowSize.y - p.mRadius)
        {
            p.mPosition.y = windowSize.y - p.mRadius;
            p.mVelocity.y = -p.mVelocity.y * COLLISION_DAMPING;
        }
    }
}

float Physics::smoothingKernel(float aRadius, float aDistance){
    float value = std::max(0.0f, aRadius * aRadius - aDistance * aDistance);
    return value * value * value;
}

float Physics::calculateDensity(const Particle &aParticle){
    float density = 0.0f;
    const float mass = 1.0f;


    for (const auto &p : mParticles)
    {
        float distance = std::hypot(p.mPosition.x - aParticle.mPosition.x, p.mPosition.y - aParticle.mPosition.y);
        float influence = smoothingKernel(SMOOTHING_RADIUS, distance);
        density += mass * influence;
    }
    return density;
}