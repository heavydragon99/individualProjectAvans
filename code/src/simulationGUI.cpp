#include "simulationGUI.h"

void SimulationGUI::showUI(sf::RenderWindow &window, sf::Time deltaTime, float fps)
{
    ImGui::SFML::Update(window, deltaTime);

    ImGui::SetNextWindowSize({250, 0.0f}, ImGuiCond_Always);
    ImGui::Begin("Simulation Controls");
    ImGui::PushItemWidth(100);

    showFPS(fps);

    auto &config = SimulationConfig::getInstance();
    if (config.simulationState() == SimulationState::SETUP) {
        renderControlsSetup();
    } else if (config.simulationState() == SimulationState::RUNNING || config.simulationState() == SimulationState::PAUSED) {
        renderControlsRunning();
    }

    ImGui::PopItemWidth();
    ImGui::End();
    ImGui::SFML::Render(window);
}

void SimulationGUI::renderControlsSetup()
{
    auto &config = SimulationConfig::getInstance();

    // Pause/Resume button
    if (ImGui::Button("Start")) {
        config.simulationState(SimulationState::INITIALIZING);
    }

    // Particle count
    unsigned int count = config.particleCount();
    if (ImGui::InputInt("Particle Count", (int *)&count)) {
        if (count < 1) {
            count = 1;
        }
        if (count > 1000000) {
            count = 1000000;
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
        if (smoothingRadius > 10000.f) {
            smoothingRadius = 10000.f;
        }
        config.smoothingRadius(smoothingRadius);
    }

    // Target density
    float targetDensity = config.targetDensity();
    if (ImGui::InputFloat("Target Density", &targetDensity)) {
        if (targetDensity < 0.01f) {
            targetDensity = 1.f;
        }
        if (targetDensity > 100.f) {
            targetDensity = 100.f;
        }
        config.targetDensity(targetDensity);
    }

    // Pressure multiplier
    float pressureMultiplier = config.pressureMultiplier();
    if (ImGui::InputFloat("Pressure Multiplier", &pressureMultiplier)) {
        if (pressureMultiplier < 0.05f) {
            pressureMultiplier = 0.05f;
        }
        if (pressureMultiplier > 100.f) {
            pressureMultiplier = 100.f;
        }
        config.pressureMultiplier(pressureMultiplier);
    }

    // Viscosity multiplier
    float viscosityMultiplier = config.viscosityMultiplier();
    if (ImGui::InputFloat("Viscosity Multiplier", &viscosityMultiplier)) {
        if (viscosityMultiplier < 0.0f) {
            viscosityMultiplier = 0.0f;
        }
        if (viscosityMultiplier > 100.f) {
            viscosityMultiplier = 100.f;
        }
        config.viscosityMultiplier(viscosityMultiplier);
    }

    // Gravity
    float gravity = config.gravity();
    if (ImGui::InputFloat("Gravity", &gravity)) {
        if (gravity < 0.0f) {
            gravity = 0.0f;
        }
        if (gravity > 100.f) {
            gravity = 100.f;
        }
        config.gravity(gravity);
    }
}

void SimulationGUI::renderControlsRunning()
{
    auto &config = SimulationConfig::getInstance();

    // Pause/Resume button
    if (ImGui::Button(config.simulationState() == SimulationState::RUNNING ? "Pause" : "Resume")) {
        if (config.simulationState() == SimulationState::RUNNING) {
            config.simulationState(SimulationState::PAUSED);
        } else {
            config.simulationState(SimulationState::RUNNING);
        }
    }

    // Reset button
    if (ImGui::Button("Reset")) {
        config.simulationState(SimulationState::RESET);
    }

    // Smoothing radius
    float smoothingRadius = config.smoothingRadius();
    if (ImGui::InputFloat("Smoothing Radius", &smoothingRadius))
    {
        if (smoothingRadius < 1.f)
        {
            smoothingRadius = 1.f;
        }
        if (smoothingRadius > 10000.f)
        {
            smoothingRadius = 10000.f;
        }
        config.smoothingRadius(smoothingRadius);
    }

    // Target density
    float targetDensity = config.targetDensity();
    if (ImGui::InputFloat("Target Density", &targetDensity))
    {
        if (targetDensity < 0.01f)
        {
            targetDensity = 1.f;
        }
        if (targetDensity > 100.f)
        {
            targetDensity = 100.f;
        }
        config.targetDensity(targetDensity);
    }

    // Pressure multiplier
    float pressureMultiplier = config.pressureMultiplier();
    if (ImGui::InputFloat("Pressure Multiplier", &pressureMultiplier))
    {
        if (pressureMultiplier < 0.5f)
        {
            pressureMultiplier = 0.5f;
        }
        if (pressureMultiplier > 100.f)
        {
            pressureMultiplier = 100.f;
        }
        config.pressureMultiplier(pressureMultiplier);
    }

    // Viscosity multiplier
    float viscosityMultiplier = config.viscosityMultiplier();
    if (ImGui::InputFloat("Viscosity Multiplier", &viscosityMultiplier))
    {
        if (viscosityMultiplier < 0.0f)
        {
            viscosityMultiplier = 0.0f;
        }
        if (viscosityMultiplier > 100.f)
        {
            viscosityMultiplier = 100.f;
        }
        config.viscosityMultiplier(viscosityMultiplier);
    }

    // Gravity
    float gravity = config.gravity();
    if (ImGui::InputFloat("Gravity", &gravity))
    {
        if (gravity < 0.0f)
        {
            gravity = 0.0f;
        }
        if (gravity > 100.f)
        {
            gravity = 100.f;
        }
        config.gravity(gravity);
    }
}

void SimulationGUI::showFPS(float fps)
{
    static int frameCounter = 0;
    static float displayedFPS = 0.0f;

    frameCounter++;
    if (frameCounter >= 30) {
        displayedFPS = fps;
        frameCounter = 0;
    }

    ImGui::Text("FPS: %.1f", displayedFPS);
}
