#include "renderer.h"

#include "simulationConfig.h"

#include <cmath>
#include <iostream>
Renderer::Renderer()
    : mWindow(sf::VideoMode(getScreenSize()), "Fluid Simulation")
{
    mWindow.setFramerateLimit(60);

    // Create a render texture for off-screen drawing.
    if (!mRenderTexture.resize(SimulationConfig::getInstance().windowSize()))
    {
        std::cerr << "Failed to create render texture!" << std::endl;
    }
}

void Renderer::clear()
{
    mWindow.clear(sf::Color::Black);
}

void Renderer::render(ParticleSystem &particleSystem)
{
    sf::Vector2u gameSize = SimulationConfig::getInstance().gameSize();

    sf::View view;
    view.setSize({static_cast<float>(gameSize.x), static_cast<float>(gameSize.y)});
    view.setCenter({static_cast<float>(gameSize.x) / 2.f, static_cast<float>(gameSize.y) / 2.f});
    view.setViewport(sf::FloatRect({0.f, 0.f}, {1.f, 1.f}));

    mRenderTexture.setView(view);
    mRenderTexture.clear(sf::Color::Transparent);

    // Draw simulation elements
    drawGrid();
    drawBorder();
    drawParticles(particleSystem);
    mRenderTexture.display();

    // Reset default view to draw the render texture to window
    mWindow.setView(mWindow.getDefaultView());

    // Draw the render texture to window
    sf::Sprite sprite(mRenderTexture.getTexture());

    mWindow.draw(sprite);
}

void Renderer::display()
{
    mWindow.display();
}

bool Renderer::isWindowOpen() const
{
    return mWindow.isOpen();
}

void Renderer::close()
{
    mWindow.close();
}

sf::Vector2u Renderer::getScreenSize() const
{
    return sf::VideoMode::getDesktopMode().size;
}

void Renderer::drawParticles(const ParticleSystem &particleSystem)
{
    sf::Vector2f maxVelocity = {100.0f, 100.0f};
    sf::Vector2f minVelocity = {0.0f, 0.0f};

    float maxSpeedMagnitude = std::sqrt(maxVelocity.x * maxVelocity.x + maxVelocity.y * maxVelocity.y);
    float minSpeedMagnitude = std::sqrt(minVelocity.x * minVelocity.x + minVelocity.y * minVelocity.y);

    sf::CircleShape particle;
    particle.setRadius(particleSystem.getParticles().front().mRadius);
    particle.setOrigin({particleSystem.getParticles().front().mRadius, particleSystem.getParticles().front().mRadius});

    for (const auto &p : particleSystem.getParticles())
    {
        // Calculate the velocity magnitude of the particle
        float velocityMagnitude = std::sqrt(p.mVelocity.x * p.mVelocity.x + p.mVelocity.y * p.mVelocity.y);
        velocityMagnitude -= minSpeedMagnitude;               // Normalize to start from 0
        velocityMagnitude = std::max(0.f, velocityMagnitude); // Ensure it's not negative

        // Normalize the velocity magnitude to a range of 0 to 1
        float normalizedVelocity = std::clamp(velocityMagnitude / maxSpeedMagnitude, 0.0f, 1.f);

        // Interpolate color based on normalized velocity
        uint8_t red = 0, green = 0, blue = 0;
        if (normalizedVelocity < 0.5f)
        {
            // From blue to green
            float t = normalizedVelocity / 0.5f;
            blue = static_cast<uint8_t>(255 * (1.f - t));
            green = static_cast<uint8_t>(255 * t);
        }
        else if (normalizedVelocity < 0.75f)
        {
            // From green to yellow
            float t = (normalizedVelocity - 0.5f) / 0.25f;
            green = 255;
            red = static_cast<uint8_t>(255 * t);
        }
        else
        {
            // From yellow to red
            float t = (normalizedVelocity - 0.75f) / 0.25f;
            red = 255;
            green = static_cast<uint8_t>(255 * (1.f - t));
        }

        particle.setFillColor(sf::Color(red, green, blue));

        particle.setPosition(p.mPosition);
        mRenderTexture.draw(particle);
    }
}

void Renderer::drawGrid()
{
    sf::Vector2u gameSize = SimulationConfig::getInstance().gameSize();
    float gridSize = SimulationConfig::getInstance().smoothingRadius();

    // Draw vertical lines
    for (float x = 0; x <= gameSize.x; x += gridSize)
    {
        std::array line = {
            sf::Vertex(sf::Vector2f(x, 0), sf::Color(100, 100, 100)),
            sf::Vertex(sf::Vector2f(x, gameSize.y), sf::Color(100, 100, 100))};
        mRenderTexture.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
    }

    // Draw horizontal lines
    for (float y = 0; y <= gameSize.y; y += gridSize)
    {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(0, y), sf::Color(100, 100, 100)),
            sf::Vertex(sf::Vector2f(gameSize.x, y), sf::Color(100, 100, 100))};
        mRenderTexture.draw(line, 2, sf::PrimitiveType::Lines);
    }
}

void Renderer::drawBorder()
{
    // Draw a red line of 1 game unit around the edge
    sf::RectangleShape border(sf::Vector2f(SimulationConfig::getInstance().gameSize().x - 2, SimulationConfig::getInstance().gameSize().y - 2));
    border.setPosition({1, 1});
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(3);
    border.setOutlineColor(sf::Color::Red);
    mRenderTexture.draw(border);
}
