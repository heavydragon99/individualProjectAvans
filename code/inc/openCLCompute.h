#ifndef TEMP_PHYSICS_H
#define TEMP_PHYSICS_H

#include <opencl.hpp>
#include <vector>
#include "particle.h"

class OpenCLCompute
{
public:
    OpenCLCompute(std::vector<Particle> &aParticles);
    ~OpenCLCompute() = default;

    // Updates particle positions on GPU
    void updatePositionsGPU(float aDeltaTime);

private:
    cl::Context mContext;
    cl::CommandQueue mQueue;
    cl::Program mProgram;
    cl::Kernel mKernelUpdatePositions;

    std::vector<Particle> &mParticles;
};

#endif // TEMP_PHYSICS_H
