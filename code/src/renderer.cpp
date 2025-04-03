#include "renderer.h"

#include "simulationConfig.h"

#include <iostream>

Renderer::Renderer()
    : mWindow(sf::VideoMode(getScreenSize()), "Fluid Simulation")
{
    mWindow.setFramerateLimit(60);

    // Create a render texture for off-screen drawing.
    if (!mRenderTexture.resize(SimulationConfig::getInstance().gameSize()))
    {
        std::cerr << "Failed to create render texture!" << std::endl;
    }

    // // Load the fragment shader
    // if (!mFluidShader.loadFromFile("../fluid_shader.frag", sf::Shader::Type::Fragment))
    // {
    //     std::cerr << "Failed to load shader!" << std::endl;
    // }
    // // Pass the resolution uniform to the shader.
    // mFluidShader.setUniform("resolution", sf::Vector2f(gameSize.x, gameSize.y));
}

void Renderer::clear()
{
    mWindow.clear(sf::Color::Black);
}

void Renderer::render(ParticleSystem &particleSystem)
{
    mRenderTexture.clear(sf::Color::Transparent);

    // Draw the particles into the off-screen render texture.
    drawGrid();
    drawBorder();
    drawParticles(particleSystem);
    mRenderTexture.display();

    // Draw the render texture with the fluid shader applied.
    sf::Sprite sprite(mRenderTexture.getTexture());

    // Calculate the scale factors while maintaining the aspect ratio.
    float scaleX = static_cast<float>(mWindow.getSize().x) / SimulationConfig::getInstance().gameSize().x;
    float scaleY = static_cast<float>(mWindow.getSize().y) / SimulationConfig::getInstance().gameSize().y;
    float scale = std::min(scaleX, scaleY); // Use the smaller scale factor to maintain aspect ratio.
    sprite.setScale({scale, scale});

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

void Renderer::resize(const sf::Vector2u &screenSize)
{
    sf::Vector2u gameSize = SimulationConfig::getInstance().gameSize();
    sf::View view = getLetterboxView(gameSize, screenSize.x, screenSize.y);
    mWindow.setView(view);

    // Update shader uniform.
    // mFluidShader.setUniform("resolution", sf::Vector2f(mGameSize.x, mGameSize.y));

    // Update the window size.
    SimulationConfig::getInstance().windowSize({screenSize.x, screenSize.y});
}

void Renderer::close()
{
    mWindow.close();
}

sf::Vector2u Renderer::getScreenSize() const
{
    return sf::VideoMode::getDesktopMode().size;
}

sf::View Renderer::getLetterboxView(const sf::Vector2u &gameSize, int windowWidth, int windowHeight)
{
    // Create the default view with game dimensions.
    sf::View view(sf::FloatRect({0.f, 0.f}, {(float)gameSize.x, (float)gameSize.y}));

    float windowAspect = static_cast<float>(windowWidth) / windowHeight;
    float gameAspect = static_cast<float>(gameSize.x) / gameSize.y;

    if (windowAspect < gameAspect)
    {
        // Window is narrower than game; adjust width.
        float newWidth = gameSize.y * windowAspect;
        view.setSize({newWidth, static_cast<float>(gameSize.y)});
        // Center horizontally in the viewport.
        float viewportX = (gameSize.x - newWidth) / (2 * gameSize.x);
        view.setViewport(sf::FloatRect({viewportX, 0.f}, {newWidth / gameSize.x, 1.f}));
    }
    else
    {
        // Window is wider than game; adjust height.
        float newHeight = gameSize.x / windowAspect;
        view.setSize({static_cast<float>(gameSize.x), newHeight});
        // Center vertically in the viewport.
        float viewportY = (gameSize.y - newHeight) / (2 * gameSize.y);
        view.setViewport(sf::FloatRect({0.f, viewportY}, {1.f, newHeight / gameSize.y}));
    }

    return view;
}

void Renderer::drawParticles(const ParticleSystem &particleSystem)
{
    sf::CircleShape particle;
    particle.setFillColor(sf::Color::White);
    particle.setRadius(particleSystem.getParticles().front().mRadius);
    particle.setOrigin({particleSystem.getParticles().front().mRadius, particleSystem.getParticles().front().mRadius});
    for (const auto &p : particleSystem.getParticles())
    {
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
    border.setOutlineThickness(1);
    border.setOutlineColor(sf::Color::Red);
    mRenderTexture.draw(border);
}
