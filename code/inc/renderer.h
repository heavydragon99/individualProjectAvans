#ifndef RENDERER_H
#define RENDERER_H

#include <SFML/Graphics.hpp>
#include "particleSystem.h"

class Renderer
{
public:
    Renderer();

    void draw(sf::RenderTarget &target, const ParticleSystem &particleSystem);
    void clear();
    void render(ParticleSystem &particleSystem);
    void display();
    bool isWindowOpen() const;
    void resize(const sf::Vector2u &screenSize);
    void close();
    sf::RenderWindow &getWindow() { return mWindow; }

private:
    sf::RenderWindow mWindow;
    sf::RenderTexture mRenderTexture;
    // sf::Shader mFluidShader;

    sf::Vector2u getScreenSize() const;
    sf::View getLetterboxView(const sf::Vector2u &gameSize, int windowWidth, int windowHeight);
};

#endif // RENDERER_H
