#include "physics.h"

#include "simulationConfig.h"

#include <cmath>
#include <algorithm>
#include <random>

extern "C"
{
    double calculatePosition(double initialPosition, double initialVelocity, double time, double acceleration)
    {
        return initialPosition + initialVelocity * time + 0.5 * acceleration * time * time;
    }
}

Physics::Physics(std::vector<Particle> &aParticles) : mParticles(aParticles) {}

void Physics::update(sf::Time aDeltaTime)
{
    applyGravity(aDeltaTime);
    updateDensities();
    checkBoundary();
    updateForces(aDeltaTime);
}

void Physics::applyGravity(sf::Time aDeltaTime)
{
    for (auto &particle : mParticles)
    {
        particle.mVelocity.y += GRAVITY * aDeltaTime.asSeconds();          // Apply gravity.
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

void Physics::updateDensities()
{
    mDensities.clear();
    for (size_t particleIndex = 0; particleIndex < mParticles.size(); ++particleIndex)
    {
        mDensities.push_back(calculateDensity(particleIndex));
    }
}

void Physics::updateForces(sf::Time aDeltaTime)
{
    for (size_t particleIndex = 0; particleIndex < mParticles.size(); ++particleIndex)
    {
        sf::Vector2f pressureForce = calculatePressureForce(particleIndex);
        sf::Vector2f pressureAcceleration = pressureForce / mDensities[particleIndex];
        mParticles[particleIndex].mVelocity += pressureAcceleration * aDeltaTime.asSeconds();
    }
}

float Physics::smoothingKernel(float aRadius, float aDistance)
{
    if (aDistance >= aRadius)
    {
        return 0.0f;
    }

    float volume =(M_PI * std::pow(aRadius, 4)) / 6;
    return (aRadius - aDistance) * (aRadius - aDistance) / volume;
}

float Physics::smoothingKernelDerivative(float aRadius, float aDistance)
{
    if (aDistance >= aRadius)
    {
        return 0.0f;
    }
    float scale = 12 / (std::pow(aRadius, 4) * M_PI);
    return (aDistance - aRadius) * scale;
}

float Physics::calculateDensity(int aParticleIndex)
{
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

sf::Vector2f Physics::calculatePressureForce(int aParticleIndex)
{
    const float mass = 10.0f;
    sf::Vector2f pressureForce{0.0f, 0.0f};
    int radius = SimulationConfig::getInstance().smoothingRadius();
    for (size_t otherParticleIndex = 0; otherParticleIndex < mParticles.size(); ++otherParticleIndex)
    {
        if (otherParticleIndex == aParticleIndex)
        {
            continue;
        }
        sf::Vector2f offset = mParticles[otherParticleIndex].mPosition - mParticles[aParticleIndex].mPosition;
        float distance = std::hypot(offset.x, offset.y);
        sf::Vector2f direction{0.0f, 0.0f};
        if (distance == 0.0f)
        {
            while (direction == sf::Vector2f{0.0f, 0.0f})
            {
                // Generate a random direction
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

                float randomX = dist(gen);
                float randomY = dist(gen);

                // Normalize the random vector
                float magnitude = std::sqrt(randomX * randomX + randomY * randomY);
                direction = magnitude == 0.0f ? sf::Vector2f{0.0f, 0.0f} : sf::Vector2f{randomX / magnitude, randomY / magnitude};
            }
        }
        else
        {
            direction = offset / distance;
        }
        float slope = smoothingKernelDerivative(radius, distance);
        float density = mDensities[otherParticleIndex];
        float sharedPressure = calculateSharedPressure(mDensities[aParticleIndex], density);
        pressureForce += -sharedPressure * direction * slope * mass / density;
    }

    return pressureForce;
}

float Physics::convertDensityToPressure(float aDensity)
{
    float densityError = aDensity - SimulationConfig::getInstance().targetDensity();
    float pressure = SimulationConfig::getInstance().pressureMultiplier() * densityError;
    return pressure;
}

float Physics::calculateSharedPressure(float aDensityA, float aDensityB)
{
    float pressureA = convertDensityToPressure(aDensityA);
    float pressureB = convertDensityToPressure(aDensityB);
    return (pressureA + pressureB) / 2;
}