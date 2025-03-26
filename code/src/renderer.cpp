#include "renderer.h"

Renderer::Renderer(float particleRadius)
    : mParticleRadius(particleRadius)
{
}

void Renderer::draw(sf::RenderTarget &target, const ParticleSystem &particleSystem)
{
    sf::CircleShape shape(mParticleRadius);
    shape.setFillColor(sf::Color::White);
    shape.setOrigin({mParticleRadius, mParticleRadius});

    for (const auto &p : particleSystem.getParticles())
    {
        shape.setPosition(p.position);
        target.draw(shape);
    }
}
