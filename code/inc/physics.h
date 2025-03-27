#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include "particle.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Clock.hpp>

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

    void update(sf::Time aDeltaTime, const sf::Vector2u &aWindowSize);

private:

    void updateParticles(sf::Time deltaTime, const sf::Vector2u &windowSize);

private:
    std::vector<Particle> &mParticles;
};

#endif // PHYSICS_H
