#ifndef FLUID_SIMULATION_H
#define FLUID_SIMULATION_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include "particleSystem.h"
#include "renderer.h"

#define GRAVITY 9.8f
#define DELTA_TIME 0.1f
#define COLLISION_DAMPING 0.8f

class FluidSimulation
{
public:
    FluidSimulation();

    void run();

private:
    void processEvents();
    void update();
    void render();

    sf::RenderWindow mWindow;
    sf::RenderTexture mRenderTexture;
    sf::Shader mFluidShader;
    ParticleSystem mParticleSystem;
    Renderer mRenderer;
};

#endif // FLUID_SIMULATION_H