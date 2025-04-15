#ifndef FLUID_SIMULATION_H
#define FLUID_SIMULATION_H

#include <iostream>
#include "particleSystem.h"
#include "renderer.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include "simulationGUI.h"
class FluidSimulation
{
public:
    FluidSimulation();

    void run();

private:
    void processEvents();
    void update();
    void render(float fps);
    void showUI();

    sf::Clock mDeltaClock;
    sf::Time mDeltaTime;
    ParticleSystem mParticleSystem;
    Renderer mRenderer;
    SimulationGUI mGUI;
    SimulationState mState;
};

#endif // FLUID_SIMULATION_H