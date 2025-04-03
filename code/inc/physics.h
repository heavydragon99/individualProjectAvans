#ifndef PHYSICS_H
#define PHYSICS_H

#include "particle.h"
#include "linearQuadTree.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>

#define GRAVITY 9.8f
#define COLLISION_DAMPING 0.8f

extern "C"
{
    double calculatePosition(double initialPosition, double initialVelocity, double time, double acceleration);
}

class Physics
{
public:
    Physics(std::vector<Particle>& aParticles);

    void initialize();
    void update(sf::Time aDeltaTime);

private:
    void applyGravity(sf::Time aDeltaTime);
    void checkBoundary();
    void updateDensities();
    void updateForces(sf::Time aDeltaTime);
    void updatePositions(sf::Time aDeltaTime);

    float smoothingKernel(float aRadius, float aDistance);
    float smoothingKernelDerivative(float aRadius, float aDistance);
    float calculateDensity(int aParticleIndex);
    sf::Vector2f calculatePressureForce(int aParticleIndex);
    float convertDensityToPressure(float aDensity);
    float calculateSharedPressure(float aDensityA, float aDensityB);

private:
    LinearQuadTree mQuadTree;
    std::vector<Particle> &mParticles;
    std::vector<float> mDensities;
};

#endif // PHYSICS_H
