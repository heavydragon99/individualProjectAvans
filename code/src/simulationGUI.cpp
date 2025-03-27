#include "simulationGUI.h"

void SimulationGUI::showUI(sf::RenderWindow &window, sf::Time deltaTime)
{
    ImGui::SFML::Update(window, deltaTime);

    ImGui::SetNextWindowSize({250, 0.0f}, ImGuiCond_Always);
    ImGui::Begin("Simulation Controls");
    ImGui::PushItemWidth(100);

    renderControls();

    ImGui::PopItemWidth();
    ImGui::End();
    ImGui::SFML::Render(window);
}

void SimulationGUI::renderControls()
{
    auto &config = SimulationConfig::getInstance();

    // Pause/Resume button
    if (ImGui::Button(config.paused() ? "Resume" : "Pause")) {
        config.paused(!config.paused());
    }

    // Simulation speed
    int speed = config.simulationSpeed();
    if (ImGui::InputInt("Simulation Speed", &speed)) {
        if (speed < 1) {
            speed = 1;
        }
        if (speed > 10) {
            speed = 10;
        }
        config.simulationSpeed(speed);
    }

    // Particle count
    unsigned int count = config.particleCount();
    if (ImGui::InputInt("Particle Count", (int *)&count)) {
        if (count < 1) {
            count = 1;
        }
        if (count > 100000) {
            count = 100000;
        }
        config.particleCount(count);
    }

    // Particle radius
    float radius = config.particleRadius();
    if (ImGui::InputFloat("Particle Radius", &radius)) {
        if (radius < 1.0f) {
            radius = 1.0f;
        }
        if (radius > 20.f) {
            radius = 20.f;
        }
        config.particleRadius(radius);
    }

    // Particle spacing
    int spacing = config.particleSpacing();
    if (ImGui::InputInt("Particle Spacing", &spacing)) {
        if (spacing < 1) {
            spacing = 1;
        }
        if (spacing > 100) {
            spacing = 100;
        }
        config.particleSpacing(spacing);
    }

    // Smoothing radius
    float smoothingRadius = config.smoothingRadius();
    if (ImGui::InputFloat("Smoothing Radius", &smoothingRadius)) {
        if (smoothingRadius < 1.f) {
            smoothingRadius = 1.f;
        }
        if (smoothingRadius > 100.f) {
            smoothingRadius = 100.f;
        }
        config.smoothingRadius(smoothingRadius);
    }
}
