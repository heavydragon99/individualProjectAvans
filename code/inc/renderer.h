#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include "particleSystem.h"

class Renderer
{
public:
    Renderer(float particleRadius);

    void draw(sf::RenderTarget &target, const ParticleSystem &particleSystem);

private:
    float mParticleRadius;
};

#endif // RENDERER_H
