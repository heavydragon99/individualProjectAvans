#include "physics.h"

extern "C" {
    double calculatePosition(double initialPosition, double initialVelocity, double time, double acceleration) {
        return initialPosition + initialVelocity * time + 0.5 * acceleration * time * time;
    }
}

void updateParticles(std::vector<Particle> &particles, float deltaTime, const sf::Vector2u &windowSize) {
    for (auto &p : particles) {
        p.mVelocity.y += GRAVITY * deltaTime;  // Apply gravity.
        p.mPosition += p.mVelocity * deltaTime; // Update position.

        // Simple boundary collision detection.
        if (p.mPosition.x < p.mRadius) {
            p.mPosition.x = p.mRadius;
            p.mVelocity.x = -p.mVelocity.x * COLLISION_DAMPING;
        } else if (p.mPosition.x > windowSize.x - p.mRadius) {
            p.mPosition.x = windowSize.x - p.mRadius;
            p.mVelocity.x = -p.mVelocity.x * COLLISION_DAMPING;
        }
        if (p.mPosition.y < p.mRadius) {
            p.mPosition.y = p.mRadius;
            p.mVelocity.y = -p.mVelocity.y * COLLISION_DAMPING;
        } else if (p.mPosition.y > windowSize.y - p.mRadius) {
            p.mPosition.y = windowSize.y - p.mRadius;
            p.mVelocity.y = -p.mVelocity.y * COLLISION_DAMPING;
        }
    }
}
