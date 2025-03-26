#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>

class Particle
{
public:
    sf::Vector2f mPosition;
    sf::Vector2f mVelocity;
    float mRadius;

    Particle(const sf::Vector2f &aPos, const sf::Vector2f &aVel, float aRadius = 1)
        : mPosition(aPos), mVelocity(aVel), mRadius(aRadius) {}
};

#endif // PARTICLE_H