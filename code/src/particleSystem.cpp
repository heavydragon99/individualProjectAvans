#include "particleSystem.h"

#include <cmath>

ParticleSystem::ParticleSystem(unsigned int aNumParticles, const sf::Vector2u &aWindowSize, float aParticleRadius)
    : mParticleRadius(aParticleRadius)
{
    // Create a grid of particles.
    int particlesPerRow = static_cast<int>(std::sqrt(aNumParticles));
    int particlesPerColumn = (aNumParticles - 1) / particlesPerRow + 1;
    float particleSpacing = 10.f;
    float spacing = mParticleRadius * 2 + particleSpacing;

    // Calculate offsets to center the grid.
    float offsetX = aWindowSize.x * 0.5f;
    float offsetY = aWindowSize.y * 0.5f;

    for (unsigned int i = 0; i < aNumParticles; i++)
    {
        float x = (i % particlesPerRow - particlesPerRow * 0.5f + 0.5f) * spacing + offsetX;
        float y = (i / particlesPerRow - particlesPerColumn * 0.5f + 0.5f) * spacing + offsetY;
        mParticles.emplace_back(sf::Vector2f(x, y), sf::Vector2f(0.f, 0.f));
    }
}

// Updates all particles: applies gravity, updates position, and handles simple boundary collisions.
void ParticleSystem::update(float aDeltaTime, float aGravity, float aCollisionDamping, const sf::Vector2u &aWindowSize)
{
    for (auto &p : mParticles)
    {
        p.velocity.y += aGravity * aDeltaTime;  // Apply gravity.
        p.position += p.velocity * aDeltaTime; // Update position.

        // Simple boundary collision detection.
        if (p.position.x < mParticleRadius)
        {
            p.position.x = mParticleRadius;
            p.velocity.x = -p.velocity.x * aCollisionDamping;
        }
        else if (p.position.x > aWindowSize.x - mParticleRadius)
        {
            p.position.x = aWindowSize.x - mParticleRadius;
            p.velocity.x = -p.velocity.x * aCollisionDamping;
        }
        if (p.position.y < mParticleRadius)
        {
            p.position.y = mParticleRadius;
            p.velocity.y = -p.velocity.y * aCollisionDamping;
        }
        else if (p.position.y > aWindowSize.y - mParticleRadius)
        {
            p.position.y = aWindowSize.y - mParticleRadius;
            p.velocity.y = -p.velocity.y * aCollisionDamping;
        }
    }
}

// Draws each particle to the target.
void ParticleSystem::draw(sf::RenderTarget &aTarget)
{
    sf::CircleShape shape(mParticleRadius);
    // Draw in white so that the shader can colorize or blend them.
    shape.setFillColor(sf::Color::White);
    shape.setOrigin({mParticleRadius, mParticleRadius});

    for (const auto &p : mParticles)
    {
        shape.setPosition(p.position);
        aTarget.draw(shape);
    }
}