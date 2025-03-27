#ifndef FLUID_SIMULATION_H
#define FLUID_SIMULATION_H

#include <iostream>
#include "particleSystem.h"
#include "renderer.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

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
    void showUI();

    // Simulation Parameters
    bool mPaused ;
    unsigned int mParticleCount;
    float mParticleRadius;
    int mSimulationSpeed;
    int mParticleSpacing;

    sf::Clock mDeltaClock;
    sf::Time mDeltaTime;
    ParticleSystem mParticleSystem;
    Renderer mRenderer;
};

#endif // FLUID_SIMULATION_H