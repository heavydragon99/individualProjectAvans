#include "physicsGPU.h"
#include "simulationConfig.h"

#include <fstream>
#include <iostream>
#include <numeric>
#include <cmath>

PhysicsGPU::PhysicsGPU(std::vector<Particle> &particles)
    : mParticles(particles), mCount(particles.size())
{
    setupOpenCL();
}

void PhysicsGPU::initialize()
{
    int gameWidth = SimulationConfig::getInstance().gameSize().x;
    int gameHeight = SimulationConfig::getInstance().gameSize().y;
    int smoothingRadius = SimulationConfig::getInstance().smoothingRadius();
    int cellCountX = gameWidth / (smoothingRadius);
    int cellCountY = gameHeight / (smoothingRadius);
    int cellCount = cellCountX * cellCountY;

    mCount = SimulationConfig::getInstance().particleCount();
    // Create buffers
    mBufPositions = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_float2) * mCount);
    mBufVelocities = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_float2) * mCount);
    mBufDensities = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(float) * mCount);
    mBufPredictedPositions = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_float2) * mCount);

    mBufPredictedPositionsSorted = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_float2) * mCount);
    mBufVelocitiesSorted = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_float2) * mCount);
    mBufDensitiesSorted = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(float) * mCount);
    mBufPositionsSorted = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_float2) * mCount);

    mBufMortonCodes = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_uint) * mCount);
    mBufSortIndices = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_uint) * mCount);
    mBufCellStart = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_int) * mCount * cellCount);
    mBufCellEnd = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_int) * mCount * cellCount);
    mBufNeighbors = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_int) * mCount * MAX_NEIGHBORS);
    mBufNeighborCounts = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_int) * mCount);
}

void PhysicsGPU::update(float dt)
{
    cl_int err;
    // Upload host data to GPU
    std::vector<cl_float2>
        positions(mCount), velocities(mCount);
    for (size_t i = 0; i < mCount; ++i)
    {
        positions[i] = {mParticles[i].mPosition.x, mParticles[i].mPosition.y};
        velocities[i] = {mParticles[i].mVelocity.x, mParticles[i].mVelocity.y};
    }
    mQueue.enqueueWriteBuffer(mBufPositions, CL_TRUE, 0, sizeof(cl_float2) * mCount, positions.data());
    mQueue.enqueueWriteBuffer(mBufVelocities, CL_TRUE, 0, sizeof(cl_float2) * mCount, velocities.data());

    // Predict positions
    mKernelPredictPositions.setArg(0, mBufPositions);
    mKernelPredictPositions.setArg(1, mBufVelocities);
    mKernelPredictPositions.setArg(2, mBufPredictedPositions);
    err = mQueue.enqueueNDRangeKernel(mKernelPredictPositions, cl::NullRange, cl::NDRange(mCount), cl::NullRange);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Kernel launch failed (predictPositions): " << err << std::endl;
    }

    // Compute Morton codes
    mKernelComputeMorton.setArg(0, mBufPredictedPositions);
    mKernelComputeMorton.setArg(1, mBufMortonCodes);
    mKernelComputeMorton.setArg(2, mBufSortIndices);
    mKernelComputeMorton.setArg(3, SimulationConfig::getInstance().smoothingRadius());
    mKernelComputeMorton.setArg(4, (int)mCount);
    err = mQueue.enqueueNDRangeKernel(mKernelComputeMorton, cl::NullRange, cl::NDRange(mCount), cl::NullRange);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Kernel launch failed (computeMorton): " << err << std::endl;
    }

    // Sort buffer
    const int logN = (int)std::log2(mCount);
    for (int stage = 1; stage <= logN; ++stage)
    {
        for (int pass = 1; pass <= stage; ++pass)
        {
            mKernelBitonicSort.setArg(0, mBufMortonCodes);
            mKernelBitonicSort.setArg(1, mBufSortIndices);
            mKernelBitonicSort.setArg(2, stage);
            mKernelBitonicSort.setArg(3, pass);
            err = mQueue.enqueueNDRangeKernel(mKernelBitonicSort, cl::NullRange, cl::NDRange(mCount / 2), cl::NullRange);
            if (err != CL_SUCCESS)
            {
                std::cerr << "Kernel launch failed (bitonicSort): " << err << std::endl;
            }
        }
    }

    // Build cell bounds
    mKernelBuildCellBounds.setArg(0, mBufMortonCodes);
    mKernelBuildCellBounds.setArg(1, mBufCellStart);
    mKernelBuildCellBounds.setArg(2, mBufCellEnd);
    mKernelBuildCellBounds.setArg(3, (int)mCount);
    err = mQueue.enqueueNDRangeKernel(mKernelBuildCellBounds, cl::NullRange, cl::NDRange(mCount), cl::NullRange);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Kernel launch failed (buildCellBounds): " << err << std::endl;
    }

    // Reorder predicted positions
    mKernelReorderParticles.setArg(0, mBufPredictedPositions);
    mKernelReorderParticles.setArg(1, mBufPredictedPositionsSorted);
    mKernelReorderParticles.setArg(2, mBufSortIndices);
    mQueue.enqueueNDRangeKernel(mKernelReorderParticles, cl::NullRange, cl::NDRange(mCount), cl::NullRange);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Kernel launch failed (reorderPredictedPositions): " << err << std::endl;
    }

    // Reorder velocities
    mKernelReorderParticles.setArg(0, mBufVelocities);
    mKernelReorderParticles.setArg(1, mBufVelocitiesSorted);
    mKernelReorderParticles.setArg(2, mBufSortIndices);
    mQueue.enqueueNDRangeKernel(mKernelReorderParticles, cl::NullRange, cl::NDRange(mCount), cl::NullRange);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Kernel launch failed (reorderVelocities): " << err << std::endl;
    }

    // Reorder positions
    mKernelReorderParticles.setArg(0, mBufPositions);
    mKernelReorderParticles.setArg(1, mBufPositionsSorted);
    mKernelReorderParticles.setArg(2, mBufSortIndices);
    mQueue.enqueueNDRangeKernel(mKernelReorderParticles, cl::NullRange, cl::NDRange(mCount), cl::NullRange);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Kernel launch failed (reorderPositions): " << err << std::endl;
    }

    // Neighbor search
    mKernelNeighborSearch.setArg(0, mBufPredictedPositionsSorted);
    mKernelNeighborSearch.setArg(1, mBufMortonCodes);
    mKernelNeighborSearch.setArg(2, mBufCellStart);
    mKernelNeighborSearch.setArg(3, mBufCellEnd);
    mKernelNeighborSearch.setArg(4, mBufNeighbors);
    mKernelNeighborSearch.setArg(5, mBufNeighborCounts);
    mKernelNeighborSearch.setArg(6, SimulationConfig::getInstance().smoothingRadius());
    mKernelNeighborSearch.setArg(7, SimulationConfig::getInstance().smoothingRadius());
    mKernelNeighborSearch.setArg(8, (int)mCount);
    err = mQueue.enqueueNDRangeKernel(mKernelNeighborSearch, cl::NullRange, cl::NDRange(mCount), cl::NullRange);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Kernel launch failed (neighborSearch): " << err << std::endl;
    }

    // Compute densities
    mKernelComputeDensity.setArg(0, mBufPredictedPositionsSorted);
    mKernelComputeDensity.setArg(1, mBufDensitiesSorted);
    mKernelComputeDensity.setArg(2, mBufNeighbors);
    mKernelComputeDensity.setArg(3, mBufNeighborCounts);
    mKernelComputeDensity.setArg(4, (int)mCount);
    mKernelComputeDensity.setArg(5, SimulationConfig::getInstance().smoothingRadius());
    err = mQueue.enqueueNDRangeKernel(mKernelComputeDensity, cl::NullRange, cl::NDRange(mCount), cl::NullRange);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Kernel launch failed (computeDensity): " << err << std::endl;
    }

    // Integrate forces & positions
    mKernelIntegrate.setArg(0, mBufPositionsSorted);
    mKernelIntegrate.setArg(1, mBufVelocitiesSorted);
    mKernelIntegrate.setArg(2, mBufPredictedPositionsSorted);
    mKernelIntegrate.setArg(3, mBufDensitiesSorted);
    mKernelIntegrate.setArg(4, mBufNeighbors);
    mKernelIntegrate.setArg(5, mBufNeighborCounts);
    mKernelIntegrate.setArg(6, (float)dt);
    mKernelIntegrate.setArg(7, (float)SimulationConfig::getInstance().targetDensity());
    mKernelIntegrate.setArg(8, (float)SimulationConfig::getInstance().pressureMultiplier());
    mKernelIntegrate.setArg(9, (float)SimulationConfig::getInstance().viscosityMultiplier());
    mKernelIntegrate.setArg(10, (float)SimulationConfig::getInstance().gravity());
    mKernelIntegrate.setArg(11, (float)SimulationConfig::getInstance().smoothingRadius());
    mKernelIntegrate.setArg(12, (float)mParticles[0].mRadius);
    mKernelIntegrate.setArg(13, (float)SimulationConfig::getInstance().gameSize().y);
    mKernelIntegrate.setArg(14, (float)SimulationConfig::getInstance().gameSize().x);
    err = mQueue.enqueueNDRangeKernel(mKernelIntegrate, cl::NullRange, cl::NDRange(mCount), cl::NullRange);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Kernel launch failed (integrate): " << err << std::endl;
    }

    mQueue.finish();

    // Download positions & velocities
    mQueue.enqueueReadBuffer(mBufPositionsSorted, CL_TRUE, 0, sizeof(cl_float2) * mCount, positions.data());
    mQueue.enqueueReadBuffer(mBufVelocitiesSorted, CL_TRUE, 0, sizeof(cl_float2) * mCount, velocities.data());

    std::vector<int> sortedIndices(mCount), inverseIndices(mCount);
    mQueue.enqueueReadBuffer(mBufSortIndices, CL_TRUE, 0, sizeof(int) * mCount, sortedIndices.data());
    for (size_t i = 0; i < mCount; ++i)
    {
        inverseIndices[sortedIndices[i]] = i;
    }


    for (size_t i = 0; i < mCount; ++i)
    {
        int sortedIdx = inverseIndices[i];
        mParticles[i].mPosition.x = positions[sortedIdx].s[0];
        mParticles[i].mPosition.y = positions[sortedIdx].s[1];
        mParticles[i].mVelocity.x = velocities[sortedIdx].s[0];
        mParticles[i].mVelocity.y = velocities[sortedIdx].s[1];
    }
}

void PhysicsGPU::setupOpenCL()
{
    // Select platform and device
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform platform = platforms.front();

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    cl::Device device = devices.front();

    // Create context and command queue
    mContext = cl::Context(device);
    mQueue = cl::CommandQueue(mContext, device);

    // Load and compile the OpenCL kernel
    std::string source = loadKernelSource("physicsKernel.cl");
    cl::Program::Sources src{{source.c_str(), source.size()}};
    mProgram = cl::Program(mContext, src);
    mProgram.build({device});

    // Always show the build log
    std::cout << "Build log:\n"
              << mProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device) << std::endl;

    // Create kernels
    mKernelComputeDensity = cl::Kernel(mProgram, "computeDensity");
    mKernelIntegrate = cl::Kernel(mProgram, "integrate");
    mKernelPredictPositions = cl::Kernel(mProgram, "predictPositions");
    mKernelComputeMorton = cl::Kernel(mProgram, "computeMorton");
    mKernelBuildCellBounds = cl::Kernel(mProgram, "buildCellBounds");
    mKernelNeighborSearch = cl::Kernel(mProgram, "neighborSearch");
    mKernelBitonicSort = cl::Kernel(mProgram, "bitonicSort");
    mKernelReorderParticles = cl::Kernel(mProgram, "reorderParticles");
}

std::string PhysicsGPU::loadKernelSource(const std::string &path)
{
    // Load the OpenCL kernel source code from a file
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open kernel file: " + path);
    return std::string(std::istreambuf_iterator<char>(file), {});
}