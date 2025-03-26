#include "fluidSimulation.h"

#include "physics.h"
#include <SFML/Window/Event.hpp>

FluidSimulation::FluidSimulation()
    : mPaused(false),
      mParticleCount(100),
      mParticleRadius(5.f),
      mSimulationSpeed(1),
      mParticleSystem(mParticleCount, {GAME_SIZE_X, GAME_SIZE_Y}, mParticleRadius),
      mRenderer(5.f, {GAME_SIZE_X, GAME_SIZE_Y})
{
    if (!ImGui::SFML::Init(mRenderer.getWindow()))
    {
        std::cerr << "Failed to initialize ImGui-SFML" << std::endl;
        return;
    }
}

void FluidSimulation::run()
{
    const sf::Time updateInterval = sf::seconds(1.f / 60.f);
    mDeltaTime = mDeltaClock.restart();
    sf::Clock guiClock;
    sf::Time timeSinceLastGuiUpdate = sf::Time::Zero;

    while (mRenderer.isWindowOpen())
    {
        mDeltaTime = mDeltaClock.restart() * (float)mSimulationSpeed;
        processEvents();
        update();
        timeSinceLastGuiUpdate += guiClock.restart();
        if (timeSinceLastGuiUpdate >= updateInterval)
        {
            render();
            timeSinceLastGuiUpdate = sf::Time::Zero;
        }
    }
    ImGui::SFML::Shutdown();
}

void FluidSimulation::processEvents()
{
    while (const std::optional event = mRenderer.getWindow().pollEvent())
    {
        ImGui::SFML::ProcessEvent(mRenderer.getWindow(), *event);
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
    if (!mPaused)
    {
        mParticleSystem.update(mDeltaTime, {GAME_SIZE_X, GAME_SIZE_Y});
    }
}

void FluidSimulation::render()
{
    showUI();
    mRenderer.render(mParticleSystem);
}

void FluidSimulation::showUI()
{
    ImGui::SFML::Update(mRenderer.getWindow(), mDeltaTime);
    ImGui::Begin("Simulation Controls");

    if (ImGui::Button(mPaused ? "Resume" : "Pause"))
    {
        mPaused = !mPaused;
    }

    if(ImGui::SliderInt("Simulation Speed", &mSimulationSpeed, 1, 10))
    {
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
    ImGui::SFML::Render(mRenderer.getWindow());
}