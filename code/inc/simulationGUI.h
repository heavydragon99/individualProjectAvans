#ifndef SIMULATION_GUI_H
#define SIMULATION_GUI_H

#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include "simulationConfig.h"

class SimulationGUI
{
public:
    void showUI(sf::RenderWindow& window, sf::Time deltaTime);

private:
    void renderControlsSetup();
    void renderControlsRunning();
};

#endif // SIMULATION_GUI_H
