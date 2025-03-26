#ifndef FLUID_SIMULATION_H
#define FLUID_SIMULATION_H

#include <iostream>
#include "particleSystem.h"
#include "renderer.h"

#define GRAVITY 9.8f
#define DELTA_TIME 0.1f
#define COLLISION_DAMPING 0.8f
#define GAME_SIZE_X 1920
#define GAME_SIZE_Y 1080

class FluidSimulation
{
public:
    FluidSimulation();

    void run();

private:
    void processEvents();
    void update();
    void render();

    ParticleSystem mParticleSystem;
    Renderer mRenderer;
};

#endif // FLUID_SIMULATION_H