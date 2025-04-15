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

Physics::Physics(std::vector<Particle> &aParticles) : mParticles(aParticles), mQuadTree(aParticles) {}

void Physics::initialize()
{
    mQuadTree.initialize();
    mDensities.resize(mParticles.size());
    mPredictedPositions.resize(mParticles.size());
}

void Physics::update(sf::Time aDeltaTime)
{
    mQuadTree.update();
    predictPositions(aDeltaTime);
    externalForces(aDeltaTime);
    updateDensities();
    updateForces(aDeltaTime);
    updatePositions(aDeltaTime);
    checkBoundary();
}

void Physics::externalForces(sf::Time aDeltaTime)
{
    float gravity = SimulationConfig::getInstance().gravity();
    bool isMousePressed = SimulationConfig::getInstance().isMousePressedLeft() || SimulationConfig::getInstance().isMousePressedRight();
    float forceMagnitude = 0.f;
    sf::Vector2f mousePos{0.f, 0.f};
    if (isMousePressed)
    {
        // Calculate mouse coordinates
        mousePos = SimulationConfig::getInstance().mousePosition();
        if (SimulationConfig::getInstance().isMousePressedLeft())
        {
            forceMagnitude = -40.f;
        }
        else
        {
            forceMagnitude = 10.f ;
        }
    }
    for (auto &particle : mParticles)
    {
        sf::Vector2f interactionForce = {0.f, 0.f};
        sf::Vector2f offset = mousePos - particle.mPosition;
        float sqrDistance = std::hypot(offset.x, offset.y);

        int width = SimulationConfig::getInstance().gameSize().x;
        int height = SimulationConfig::getInstance().gameSize().y;
        int radius = std::min(width * 0.1, height * 0.1);

        if (sqrDistance < radius)
        {
            float distance = std::sqrt(sqrDistance);
            float edgeT = distance / radius;
            float centreT = 1 - edgeT;
            sf::Vector2f dirToCentre = offset / distance;

            float gravityWeight = 1 - centreT;
            interactionForce = sf::Vector2f(0.f, gravity) * gravityWeight + dirToCentre * centreT * forceMagnitude;
            interactionForce -= particle.mVelocity * centreT;
        }

        // Apply gravity to all particles
        sf::Vector2f gravityForce = {0.f, gravity};
        particle.mVelocity += (gravityForce + interactionForce) * aDeltaTime.asSeconds();
    }
}

void Physics::checkBoundary()
{
    const sf::Vector2u gameSize = SimulationConfig::getInstance().gameSize();
    for (auto &particle : mParticles)
    {
        // Simple boundary collision detection.
        if (particle.mPosition.x < particle.mRadius)
        {
            particle.mPosition.x = particle.mRadius;
            particle.mVelocity.x = -particle.mVelocity.x * COLLISION_DAMPING;
        }
        else if (particle.mPosition.x > gameSize.x - particle.mRadius)
        {
            particle.mPosition.x = gameSize.x - particle.mRadius;
            particle.mVelocity.x = -particle.mVelocity.x * COLLISION_DAMPING;
        }
        if (particle.mPosition.y < particle.mRadius)
        {
            particle.mPosition.y = particle.mRadius;
            particle.mVelocity.y = -particle.mVelocity.y * COLLISION_DAMPING;
        }
        else if (particle.mPosition.y > gameSize.y - particle.mRadius)
        {
            particle.mPosition.y = gameSize.y - particle.mRadius;
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
        float density = mDensities[particleIndex];
        if (density <= 0.0f)
        {
            continue; // Skip if density is zero or negative.
        }

        sf::Vector2f pressureForce = calculatePressureForce(particleIndex);
        sf::Vector2f viscosityForce = calculateViscosityForce(particleIndex); // Add viscosity force calculation.

        sf::Vector2f totalAcceleration = (pressureForce + viscosityForce) / density;
        mParticles[particleIndex].mVelocity += totalAcceleration * aDeltaTime.asSeconds();
    }
}

void Physics::updatePositions(sf::Time aDeltaTime)
{
    for (auto &particle : mParticles)
    {
        particle.mPosition += particle.mVelocity * aDeltaTime.asSeconds();
    }
}

float Physics::smoothingKernel(float aRadius, float aDistance)
{
    if (aDistance >= aRadius)
    {
        return 0.0f;
    }

    float volume = (M_PI * std::pow(aRadius, 4)) / 6;
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

float Physics::viscosityKernel(float aRadius, float aDistance)
{
    float volume = (M_PI * std::pow(aRadius, 8)) / 4;
    float value = std::max(0.0f, aRadius * aRadius - aDistance * aDistance);
    return value * value * value / volume;
}

float Physics::calculateDensity(int aParticleIndex)
{
    float density = 0.0f;
    const float mass = 1.0f;
    float smoothingRadius = SimulationConfig::getInstance().smoothingRadius();

    Particle &sampleParticle = mParticles[aParticleIndex];
    std::vector<size_t> neighbors = mQuadTree.findNeighbors(sampleParticle, smoothingRadius);

    for (size_t neighborIndex : neighbors)
    {
        float distance = std::hypot(
            mPredictedPositions[neighborIndex].x - mPredictedPositions[aParticleIndex].x,
            mPredictedPositions[neighborIndex].y - mPredictedPositions[aParticleIndex].y);
        float influence = smoothingKernel(smoothingRadius, distance);
        density += mass * influence;
    }

    return density;
}

sf::Vector2f Physics::calculatePressureForce(int aParticleIndex)
{
    const float mass = 1.0f;
    sf::Vector2f pressureForce{0.0f, 0.0f};
    float smoothingRadius = SimulationConfig::getInstance().smoothingRadius();

    std::vector<size_t> neighbors = mQuadTree.findNeighbors(mParticles[aParticleIndex], smoothingRadius);

    for (size_t neighborIndex : neighbors)
    {
        if (&mParticles[neighborIndex] == &mParticles[aParticleIndex])
        {
            continue;
        }

        sf::Vector2f offset = mPredictedPositions[neighborIndex] - mPredictedPositions[aParticleIndex];
        float distance = std::hypot(offset.x, offset.y);
        sf::Vector2f direction = (distance == 0.0f) ? sf::Vector2f{0.0f, 0.0f} : offset / distance;

        float slope = smoothingKernelDerivative(smoothingRadius, distance);
        float density = mDensities[neighborIndex];
        if (density <= 0.0f)
        {
            continue; // Skip if density is zero or negative.
        }
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

sf::Vector2f Physics::calculateViscosityForce(int aParticleIndex)
{
    sf::Vector2f viscosityForce{0.0f, 0.0f};
    sf::Vector2f position = mParticles[aParticleIndex].mPosition;
    float smoothingRadius = SimulationConfig::getInstance().smoothingRadius();

    std::vector<size_t> neighbors = mQuadTree.findNeighbors(mParticles[aParticleIndex], smoothingRadius);
    for (size_t neighborIndex : neighbors)
    {
        if (&mParticles[neighborIndex] == &mParticles[aParticleIndex])
        {
            continue;
        }

        sf::Vector2f offset = mParticles[neighborIndex].mPosition - position;
        float distance = std::hypot(offset.x, offset.y);
        if (distance <= 0.0f)
        {
            continue; // Avoid division by zero
        }
        float influence = viscosityKernel(SimulationConfig::getInstance().smoothingRadius(), distance);
        viscosityForce += (mParticles[neighborIndex].mVelocity - mParticles[aParticleIndex].mVelocity) * influence;
    }

    return viscosityForce * SimulationConfig::getInstance().viscosityMultiplier();
}

void Physics::predictPositions(sf::Time aDeltaTime)
{
    for (size_t i = 0; i < mParticles.size(); ++i)
    {
        Particle &particle = mParticles[i];
        mPredictedPositions[i] = particle.mPosition + particle.mVelocity * 1.f / 60.f;
    }
}