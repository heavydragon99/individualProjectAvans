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
        mDeltaTime = mDeltaClock.restart() * 2.f;
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
        if (SimulationConfig::getInstance().simulationState() == SimulationState::RUNNING)
        {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
            {
                mParticleSystem.applyMouseForce(mRenderer.getWindow().mapPixelToCoords({sf::Mouse::getPosition(mRenderer.getWindow()).x, sf::Mouse::getPosition(mRenderer.getWindow()).y}), -10.f);
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
            {
                mParticleSystem.applyMouseForce(mRenderer.getWindow().mapPixelToCoords({sf::Mouse::getPosition(mRenderer.getWindow()).x, sf::Mouse::getPosition(mRenderer.getWindow()).y}), 10.f);
            }
        }
    }
}

void FluidSimulation::update()
{
    if (SimulationConfig::getInstance().simulationState() == SimulationState::RUNNING)
    {
        mParticleSystem.update(mDeltaTime);
    }
    else if (SimulationConfig::getInstance().simulationState() == SimulationState::RESET)
    {
        mParticleSystem.reset();
        SimulationConfig::getInstance().simulationState(SimulationState::SETUP);
    }
    else if (SimulationConfig::getInstance().simulationState() == SimulationState::INITIALIZING)
    {
        mParticleSystem.initialize();
        SimulationConfig::getInstance().simulationState(SimulationState::RUNNING);
    }
}

void FluidSimulation::render()
{
    mRenderer.clear();
    mRenderer.render(mParticleSystem);
    mGUI.showUI(mRenderer.getWindow(), mDeltaTime);
    mRenderer.display();
}