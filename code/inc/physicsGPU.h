#ifndef PHYSICS_GPU_H
#define PHYSICS_GPU_H

#include "particle.h"
#include <opencl.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Clock.hpp>
#include <vector>

#define COLLISION_DAMPING 0.8f
#define MAX_NEIGHBORS 100

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
    cl::Kernel mKernelComputeMorton;
    cl::Kernel mKernelBuildCellBounds;
    cl::Kernel mKernelNeighborSearch;
    cl::Kernel mKernelBitonicSort;
    cl::Kernel mKernelReorderParticles;

    cl::Buffer mBufPositions;
    cl::Buffer mBufVelocities;
    cl::Buffer mBufDensities;
    cl::Buffer mBufPredictedPositions;

    cl::Buffer mBufPositionsSorted;
    cl::Buffer mBufVelocitiesSorted;
    cl::Buffer mBufDensitiesSorted;
    cl::Buffer mBufPredictedPositionsSorted;

    cl::Buffer mBufMortonCodes;
    cl::Buffer mBufSortIndices;
    cl::Buffer mBufCellStart;
    cl::Buffer mBufCellEnd;
    cl::Buffer mBufNeighbors;
    cl::Buffer mBufNeighborCounts;

    std::vector<Particle> &mParticles;
    size_t mCount;
};

#endif // PHYSICS_GPU_H