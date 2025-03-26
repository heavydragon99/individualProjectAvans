#ifndef PHYSICS_H
#define PHYSICS_H

#include <vector>
#include "particle.h"
#include <SFML/System/Vector2.hpp>

#define GRAVITY 9.8f
#define COLLISION_DAMPING 0.8f

extern "C" {
    double calculatePosition(double initialPosition, double initialVelocity, double time, double acceleration);
}

void updateParticles(std::vector<Particle> &particles, float deltaTime, const sf::Vector2u &windowSize);

#endif // PHYSICS_H
