#include "fluidSimulation.h"

#include "physics.h"
#include <SFML/Window/Event.hpp>

FluidSimulation::FluidSimulation()
    : mPaused(false),
      mParticleCount(100),
      mParticleRadius(5.f),
      mParticleSystem(mParticleCount, {GAME_SIZE_X, GAME_SIZE_Y}, mParticleRadius),
      mRenderer(5.f, {GAME_SIZE_X, GAME_SIZE_Y})
{
    ImGui::SFML::Init(mRenderer.getWindow());
}

void FluidSimulation::run()
{
    while (mRenderer.isWindowOpen())
    {
        processEvents();
        if (!mPaused)
        {
            update();
        }
        render();
    }

    ImGui::SFML::Shutdown();
}

void FluidSimulation::processEvents()
{
    while (const std::optional event = mRenderer.getWindow().pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            mRenderer.close();
        }
        else if (const auto *resized = event->getIf<sf::Event::Resized>())
        {
            mRenderer.resize(sf::Vector2u({resized->size.x, resized->size.y}));
        }
    }
}

void FluidSimulation::update()
{
    ImGui::NewFrame();
    showUI();

    mParticleSystem.update(DELTA_TIME, {GAME_SIZE_X, GAME_SIZE_Y});
    ImGui::SFML::Render(mRenderer.getWindow());
}

void FluidSimulation::render()
{
    mRenderer.render(mParticleSystem);
}

void FluidSimulation::showUI()
{
    ImGui::Begin("Simulation Controls");

    if (ImGui::Button(mPaused ? "Resume" : "Pause"))
    {
        mPaused = !mPaused;
    }

    // Particle count slider
    if (ImGui::SliderInt("Particle Count", (int *)&mParticleCount, 1, 1000))
    {
        mParticleSystem = ParticleSystem(mParticleCount, {GAME_SIZE_X, GAME_SIZE_Y}, mParticleRadius);
    }

    // Particle radius slider
    if (ImGui::SliderFloat("Particle Radius", &mParticleRadius, 1.f, 20.f))
    {
    }

    ImGui::End();
}