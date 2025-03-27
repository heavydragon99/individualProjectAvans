#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include "particleSystem.h"

class Renderer
{
public:
    Renderer(const sf::Vector2u &gameSize);

    void draw(sf::RenderTarget &target, const ParticleSystem &particleSystem);
    void render(ParticleSystem &particleSystem);
    bool isWindowOpen() const;
    void resize(const sf::Vector2u &screenSize);
    void close();
    sf::RenderWindow &getWindow() { return mWindow; }

private:
    sf::Vector2u mGameSize;
    sf::Vector2u mWindowSize;
    sf::RenderWindow mWindow;
    sf::RenderTexture mRenderTexture;
    // sf::Shader mFluidShader;

    sf::Vector2u getScreenSize() const;
    sf::View getLetterboxView(const sf::Vector2u &gameSize, int windowWidth, int windowHeight);
};

#endif // RENDERER_H
