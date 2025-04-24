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
    const sf::Time updateIntervalGui = sf::seconds(1.f / 60.f);
    mDeltaTime = mDeltaClock.restart();
    sf::Clock guiClock;
    sf::Time timeSinceLastGuiUpdate = sf::Time::Zero;

    float fps = 0;
    sf::Clock fpsClock;

    while (mRenderer.isWindowOpen())
    {
        mDeltaTime = mDeltaClock.restart();
        if(SimulationConfig::getInstance().useGPU())
        {
            mDeltaTime *= 5.f;
        }
        else
        {
            mDeltaTime *= 2.f;
        }
        processEvents();
        update();
        timeSinceLastGuiUpdate += guiClock.restart();
        if (timeSinceLastGuiUpdate >= updateIntervalGui)
        {
            fps = 1.f / fpsClock.restart().asSeconds(); // Calculate FPS
            render(fps); // Pass FPS to render
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
            SimulationConfig::getInstance().isMousePressedLeft(false);
            SimulationConfig::getInstance().isMousePressedRight(false);
            sf::Vector2f mousePos = mRenderer.getWindow().mapPixelToCoords(sf::Mouse::getPosition(mRenderer.getWindow()));
            // Scale mousePos to the game size
            mousePos.x = (mousePos.x / mRenderer.getWindow().getSize().x) * SimulationConfig::getInstance().gameSize().x;
            mousePos.y = (mousePos.y / mRenderer.getWindow().getSize().y) * SimulationConfig::getInstance().gameSize().y;
            SimulationConfig::getInstance().mousePosition(mousePos);
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
            {
                SimulationConfig::getInstance().isMousePressedLeft(true);
            }
            else if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
            {
                SimulationConfig::getInstance().isMousePressedRight(true);
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

void FluidSimulation::render(float fps)
{
    mRenderer.clear();
    mRenderer.render(mParticleSystem);
    mGUI.showUI(mRenderer.getWindow(), mDeltaTime, fps);
    mRenderer.display();
}