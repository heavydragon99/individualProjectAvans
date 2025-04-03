#include "fluidSimulation.h"

#include "physics.h"
#include "simulationConfig.h"

#include <SFML/Window/Event.hpp>

FluidSimulation::FluidSimulation()
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
        mDeltaTime = mDeltaClock.restart() * (float)SimulationConfig::getInstance().simulationSpeed();
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
            SimulationConfig::getInstance().windowSize({resized->size.x, resized->size.y});
            mRenderer.resize(sf::Vector2u({resized->size.x, resized->size.y}));
        }
    }
}

void FluidSimulation::update()
{
    if (SimulationConfig::getInstance().simulationState() == SimulationState::RUNNING)
    {
        mParticleSystem.update(mDeltaTime);
    }
}

void FluidSimulation::render()
{
    mRenderer.clear();
    mRenderer.render(mParticleSystem);
    mGUI.showUI(mRenderer.getWindow(), mDeltaTime);
    mRenderer.display();
}