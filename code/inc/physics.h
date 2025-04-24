#ifndef PHYSICS_H
#define PHYSICS_H

#include "particle.h"
#include "linearQuadTree.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>

#define COLLISION_DAMPING 0.8f

class Physics
{
public:
    Physics(std::vector<Particle>& aParticles);

    void initialize();
    void update(sf::Time aDeltaTime);

private:
    void externalForces(sf::Time aDeltaTime);
    void checkBoundary();
    void updateDensities();
    void updateForces(sf::Time aDeltaTime);
    void updatePositions(sf::Time aDeltaTime);

    float smoothingKernel(float aRadius, float aDistance);
    float smoothingKernelDerivative(float aRadius, float aDistance);
    float viscosityKernel(float aRadius, float aDistance);
    float calculateDensity(int aParticleIndex);
    sf::Vector2f calculatePressureForce(int aParticleIndex);
    float convertDensityToPressure(float aDensity);
    float calculateSharedPressure(float aDensityA, float aDensityB);
    sf::Vector2f calculateViscosityForce(int aParticleIndex);
    void predictPositions(sf::Time aDeltaTime);

private:
    LinearQuadTree mQuadTree;
    std::vector<Particle> &mParticles;
    std::vector<float> mDensities;
    std::vector<sf::Vector2f> mPredictedPositions;
};

#endif // PHYSICS_H
