#include "fluidSimulation.h"

FluidSimulation::FluidSimulation()
    : mParticleSystem(100, {GAME_SIZE_X, GAME_SIZE_Y}, 5.f),
      mRenderer(5.f, {GAME_SIZE_X, GAME_SIZE_Y})
{
}

void FluidSimulation::run()
{
    while (mRenderer.isWindowOpen())
    {
        processEvents();
        update();
        render();
    }
}

void FluidSimulation::processEvents()
{
    mRenderer.processEvents();
}

void FluidSimulation::update()
{
    mParticleSystem.update(DELTA_TIME, GRAVITY, COLLISION_DAMPING, {GAME_SIZE_X, GAME_SIZE_Y});
}

void FluidSimulation::render()
{
    mRenderer.render(mParticleSystem);
}