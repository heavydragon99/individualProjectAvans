#ifndef PHYSICS_GPU_H
#define PHYSICS_GPU_H

#include "particle.h"
#include <opencl.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>

#define COLLISION_DAMPING 0.8f

class PhysicsGPU
{
public:
    PhysicsGPU(std::vector<Particle> &particles);
    ~PhysicsGPU() = default;

    void initialize();
    void update(float dt);

private:
    void setupOpenCL();
    std::string loadKernelSource(const std::string &path);

    // OpenCL
    cl::Context mContext;
    cl::CommandQueue mQueue;
    cl::Program mProgram;

    cl::Kernel mKernelComputeDensity;
    cl::Kernel mKernelIntegrate;
    cl::Kernel mKernelPredictPositions;

    cl::Buffer mBufPositions;
    cl::Buffer mBufVelocities;
    cl::Buffer mBufDensities;
    cl::Buffer mBufForces;
    cl::Buffer mBufPredictedPositions;

    std::vector<Particle> &mParticles;
    size_t mCount;
};

#endif // PHYSICS_GPU_H