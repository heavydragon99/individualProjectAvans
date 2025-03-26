#include "fluidSimulation.h"

FluidSimulation::FluidSimulation()
    : mWindow(sf::VideoMode({1920, 1080}), "Fluid Simulation"),
      mParticleSystem(100, mWindow.getSize(), 5.f),
      mRenderer(5.f)
{
    mWindow.setFramerateLimit(60);

    // Create a render texture for off-screen drawing.
    if (!mRenderTexture.resize({mWindow.getSize().x, mWindow.getSize().y}))
    {
        std::cerr << "Failed to create render texture!" << std::endl;
    }

    // Load the fragment shader
    if (!mFluidShader.loadFromFile("../fluid_shader.frag", sf::Shader::Type::Fragment))
    {
        std::cerr << "Failed to load shader!" << std::endl;
    }
    // Pass the resolution uniform to the shader.
    mFluidShader.setUniform("resolution", sf::Vector2f(mWindow.getSize()));
}

void FluidSimulation::run()
{
    while (mWindow.isOpen())
    {
        processEvents();
        update();
        render();
    }
}

void FluidSimulation::processEvents()
{
    while (const std::optional event = mWindow.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            mWindow.close();
        }
        else if (const auto *resized = event->getIf<sf::Event::Resized>())
        {
            // update the view to the new size of the window
            sf::FloatRect visibleArea({0.f, 0.f}, sf::Vector2f(resized->size));
            mWindow.setView(sf::View(visibleArea));
            // Resize the render texture and update shader uniform.
            mRenderTexture.resize(resized->size);
            mFluidShader.setUniform("resolution", sf::Vector2f(resized->size));
        }
    }
}

void FluidSimulation::update()
{
    mParticleSystem.update(DELTA_TIME, GRAVITY, COLLISION_DAMPING, mWindow.getSize());
}

void FluidSimulation::render()
{
    // First, draw the particles into the off-screen render texture.
    mRenderTexture.clear(sf::Color::Transparent);
    mRenderer.draw(mRenderTexture, mParticleSystem);
    mRenderTexture.display();

    // Then, clear the window and draw the render texture with the fluid shader applied.
    mWindow.clear();
    sf::Sprite sprite(mRenderTexture.getTexture());
    mWindow.draw(sprite, &mFluidShader);
    mWindow.display();
}