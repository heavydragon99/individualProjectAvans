#include "physicsGPU.h"
#include "simulationConfig.h"
#include <fstream>
#include <iostream>

PhysicsGPU::PhysicsGPU(std::vector<Particle> &particles)
    : mParticles(particles), mCount(particles.size())
{
    setupOpenCL();
}

void PhysicsGPU::initialize()
{
    mCount = SimulationConfig::getInstance().particleCount();
    // Create buffers
    mBufPositions = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_float2) * mCount);
    mBufVelocities = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_float2) * mCount);
    mBufDensities = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(float) * mCount);
    mBufForces = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_float2) * mCount);
    mBufPredictedPositions = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_float2) * mCount);
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

    // Compute densities
    mKernelComputeDensity.setArg(0, mBufPredictedPositions);
    mKernelComputeDensity.setArg(1, mBufDensities);
    mKernelComputeDensity.setArg(2, (int)mCount);
    mKernelComputeDensity.setArg(3, SimulationConfig::getInstance().smoothingRadius());
    err = mQueue.enqueueNDRangeKernel(mKernelComputeDensity, cl::NullRange, cl::NDRange(mCount), cl::NullRange);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Kernel launch failed (computeDensity): " << err << std::endl;
    }

    // Integrate forces & positions
    mKernelIntegrate.setArg(0, mBufPositions);
    mKernelIntegrate.setArg(1, mBufVelocities);
    mKernelIntegrate.setArg(2, mBufPredictedPositions);
    mKernelIntegrate.setArg(3, mBufDensities);
    mKernelIntegrate.setArg(4, (float)dt);
    mKernelIntegrate.setArg(5, (float)SimulationConfig::getInstance().targetDensity());
    mKernelIntegrate.setArg(6, (float)SimulationConfig::getInstance().pressureMultiplier());
    mKernelIntegrate.setArg(7, (float)SimulationConfig::getInstance().viscosityMultiplier());
    mKernelIntegrate.setArg(8, (float)SimulationConfig::getInstance().gravity());
    mKernelIntegrate.setArg(9, (float)SimulationConfig::getInstance().smoothingRadius());
    mKernelIntegrate.setArg(10, (float)mParticles[0].mRadius);
    mKernelIntegrate.setArg(11, (float)SimulationConfig::getInstance().gameSize().y);
    mKernelIntegrate.setArg(12, (float)SimulationConfig::getInstance().gameSize().x);
    err = mQueue.enqueueNDRangeKernel(mKernelIntegrate, cl::NullRange, cl::NDRange(mCount), cl::NullRange);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Kernel launch failed (integrate): " << err << std::endl;
    }

    mQueue.finish();

    // Download positions & velocities
    mQueue.enqueueReadBuffer(mBufPositions, CL_TRUE, 0, sizeof(cl_float2) * mCount, positions.data());
    mQueue.enqueueReadBuffer(mBufVelocities, CL_TRUE, 0, sizeof(cl_float2) * mCount, velocities.data());

    for (size_t i = 0; i < mCount; ++i)
    {
        mParticles[i].mPosition.x = positions[i].s[0];
        mParticles[i].mPosition.y = positions[i].s[1];
        mParticles[i].mVelocity.x = velocities[i].s[0];
        mParticles[i].mVelocity.y = velocities[i].s[1];
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
}

std::string PhysicsGPU::loadKernelSource(const std::string &path)
{
    // Load the OpenCL kernel source code from a file
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open kernel file: " + path);
    return std::string(std::istreambuf_iterator<char>(file), {});
}