#ifndef PARTICLE_H
#define PARTICLE_H

#include <SFML/Graphics.hpp>

class Particle
{
public:
    sf::Vector2f position;
    sf::Vector2f velocity;

    Particle(const sf::Vector2f &pos, const sf::Vector2f &vel)
        : position(pos), velocity(vel) {}
};

#endif // PARTICLE_H