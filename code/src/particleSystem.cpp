#include "particleSystem.h"

#include "physics.h"
#include "simulationConfig.h"

#include <cmath>

ParticleSystem::ParticleSystem()
{
    mPhysicsEngine = std::make_unique<Physics>(mParticles);
    mPhysicsEngineGPU = std::make_unique<PhysicsGPU>(mParticles);

    // Register callbacks
    SimulationConfig &config = SimulationConfig::getInstance();
    config.setOnParticleCountChanged([this]
                                     { updatedParticleCount(); });
    config.setOnParticleRadiusChanged([this]
                                      { updatedParticleRadius(); });
    config.setOnParticleSpacingChanged([this]
                                       { updatedParticleSpacing(); });
    spawnParticles();
                                    }

void ParticleSystem::initialize()
{
    reset();
    if(SimulationConfig::getInstance().useGPU())
    {
        mPhysicsEngineGPU->initialize();
    }
    else
    {
        mPhysicsEngine->initialize();
    }
}

void ParticleSystem::update(sf::Time aDeltaTime)
{
    if(SimulationConfig::getInstance().useGPU())
    {
        mPhysicsEngineGPU->update(aDeltaTime.asSeconds());
    }
    else
    {
        mPhysicsEngine->update(aDeltaTime);
    }
}

void ParticleSystem::reset()
{
    mParticles.clear();
    spawnParticles();
}

const std::vector<Particle> &ParticleSystem::getParticles() const
{
    return mParticles;
}

void ParticleSystem::updatedParticleCount()
{
    mParticles.clear();
    spawnParticles();
}

void ParticleSystem::updatedParticleRadius()
{
    float radius = SimulationConfig::getInstance().particleRadius();
    for (auto &p : mParticles)
    {
        p.mRadius = radius;
    }
}

void ParticleSystem::updatedParticleSpacing()
{
    mParticles.clear();
    spawnParticles();
}

void ParticleSystem::spawnParticles()
{
    SimulationConfig &config = SimulationConfig::getInstance();
    // Create a grid of particles.
    unsigned int numParticles = config.particleCount();
    int particlesPerRow = static_cast<int>(std::sqrt(numParticles));
    int particlesPerColumn = (numParticles - 1) / particlesPerRow + 1;

    // Calculate offsets to center the grid.
    sf::Vector2u gameSize = config.gameSize();
    float offsetX = gameSize.x * 0.5f;
    float offsetY = gameSize.y * 0.5f;

    float particleRadius = config.particleRadius();
    float particleSpacing = config.particleSpacing();
    for (unsigned int i = 0; i < numParticles; i++)
    {
        float x = (i % particlesPerRow - particlesPerRow * 0.5f + 0.5f) * particleSpacing + offsetX;
        float y = (i / particlesPerRow - particlesPerColumn * 0.5f + 0.5f) * particleSpacing + offsetY;
        mParticles.emplace_back(sf::Vector2f(x, y), sf::Vector2f(0.f, 0.f), particleRadius);
    }
}