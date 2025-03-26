#include "renderer.h"

#include <iostream>

Renderer::Renderer(float particleRadius, const sf::Vector2u &gameSize)
    : mParticleRadius(particleRadius), mGameSize(gameSize), mWindowSize(getScreenSize()), mWindow(sf::VideoMode(getScreenSize()), "Fluid Simulation")
{
    mWindow.setFramerateLimit(60);

    // Create a render texture for off-screen drawing.
    if (!mRenderTexture.resize(mGameSize))
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

void Renderer::draw(sf::RenderTarget &target, const ParticleSystem &particleSystem)
{
    sf::CircleShape shape(mParticleRadius);
    shape.setFillColor(sf::Color::White);
    shape.setOrigin({mParticleRadius, mParticleRadius});

    for (const auto &p : particleSystem.getParticles())
    {
        shape.setPosition(p.mPosition);
        target.draw(shape);
    }

    // Draw a red line of 1 game unit around the edge
    sf::RectangleShape border(sf::Vector2f(mGameSize.x-2, mGameSize.y-2));
    border.setPosition({1, 1});
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(1);
    border.setOutlineColor(sf::Color::Red);
    target.draw(border);
}

void Renderer::render(ParticleSystem &particleSystem)
{
    // First, draw the particles into the off-screen render texture.
    mRenderTexture.clear(sf::Color::Transparent);
    draw(mRenderTexture, particleSystem);
    mRenderTexture.display();

    // Then, clear the window and draw the render texture with the fluid shader applied.
    mWindow.clear(sf::Color::Black);
    sf::Sprite sprite(mRenderTexture.getTexture());

    // Calculate the scale factors while maintaining the aspect ratio.
    float scaleX = static_cast<float>(mWindow.getSize().x) / mGameSize.x;
    float scaleY = static_cast<float>(mWindow.getSize().y) / mGameSize.y;
    float scale = std::min(scaleX, scaleY); // Use the smaller scale factor to maintain aspect ratio.
    sprite.setScale({scale, scale});

    mWindow.draw(sprite);
    mWindow.display();
}

bool Renderer::isWindowOpen() const
{
    return mWindow.isOpen();
}

void Renderer::resize(const sf::Vector2u &screenSize)
{
    // Create a view with your game area's dimensions.
    sf::View view(sf::FloatRect({0.f, 0.f}, {mGameSize.x, mGameSize.y}));
    // Adjust the view to maintain aspect ratio with black bars.
    view = getLetterboxView(mGameSize, screenSize.x, screenSize.y);
    mWindow.setView(view);

    // Update shader uniform.
    // mFluidShader.setUniform("resolution", sf::Vector2f(mGameSize.x, mGameSize.y));

    // Update the window size.
    mWindowSize = {screenSize.x, screenSize.y};
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
    sf::View view(sf::FloatRect({0.f, 0.f}, {gameSize.x, gameSize.y}));

    float windowAspect = static_cast<float>(windowWidth) / windowHeight;
    float gameAspect = static_cast<float>(gameSize.x) / gameSize.y;

    if (windowAspect < gameAspect)
    {
        // If window is narrower than game aspect ratio -> scale height to match window height
        float newWidth = gameSize.y * windowAspect;
        view.setSize({newWidth, gameSize.y});
    }

    // Center horizontally in the viewport
    float sizeX = windowAspect / gameAspect;
    float posX = (1.f - sizeX) / 2.f;
    view.setViewport(sf::FloatRect({posX, 0.f}, {sizeX, 1.f}));

    return view;
}
