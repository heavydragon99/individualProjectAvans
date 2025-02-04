#include "Physics.h"

extern "C" {
    double calculatePosition(double initialPosition, double initialVelocity, double time, double acceleration) {
        return initialPosition + initialVelocity * time + 0.5 * acceleration * time * time;
    }
}
