#include "fluidSimulation.h"

#include "physics.h"
#include <SFML/Window/Event.hpp>

FluidSimulation::FluidSimulation()
    : mPaused(true),
      mParticleCount(100),
      mParticleRadius(5.f),
      mSimulationSpeed(5),
      mParticleSpacing(15),
      mParticleSystem(mParticleCount, {GAME_SIZE_X, GAME_SIZE_Y}, mParticleRadius, mParticleSpacing),
      mRenderer({GAME_SIZE_X, GAME_SIZE_Y})
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

    ImGui::SetNextWindowSize({250, 0.0f}, ImGuiCond_Always);
    ImGui::Begin("Simulation Controls");
    ImGui::PushItemWidth(100);

    // Pause/Resume button
    if (ImGui::Button(mPaused ? "Resume" : "Pause"))
    {
        mPaused = !mPaused;
    }

    // Simulation speed
    if (ImGui::InputInt("Simulation Speed", &mSimulationSpeed))
    {
        if (mSimulationSpeed < 1)
        {
            mSimulationSpeed = 1;
        }
        if (mSimulationSpeed > 10)
        {
            mSimulationSpeed = 10;
        }
    }

    // Particle count 
    if (ImGui::InputInt("Particle Count", (int *)&mParticleCount))
    {
        if (mParticleCount < 1)
        {
            mParticleCount = 1;
        }
        if (mParticleCount > 100000)
        {
            mParticleCount = 100000;
        }
        mParticleSystem.setParticleCount(mParticleCount);
    }

    // Particle radius
    if (ImGui::InputFloat("Particle Radius", &mParticleRadius))
    {
        if (mParticleRadius < 0.1f)
        {
            mParticleRadius = 0.1f;
        }
        if (mParticleRadius > 20.f)
        {
            mParticleRadius = 20.f;
        }
        mParticleSystem.setParticleRadius(mParticleRadius);
    }

    // Particle spacing
    if (ImGui::InputInt("Particle Spacing", &mParticleSpacing))
    {
        if (mParticleSpacing < 1)
        {
            mParticleSpacing = 1;
        }
        if (mParticleSpacing > 100)
        {
            mParticleSpacing = 100;
        }
        mParticleSystem.setParticleSpacing(mParticleSpacing);
    }

    ImGui::PopItemWidth();
    ImGui::End();
    ImGui::SFML::Render(mRenderer.getWindow());
}