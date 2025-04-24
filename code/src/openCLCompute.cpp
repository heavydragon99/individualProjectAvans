#include "openCLCompute.h"

#include <fstream>
#include <iostream>

// Helper to load OpenCL source
static std::string loadKernelSource(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Cannot open kernel file: " + path);
    return std::string(std::istreambuf_iterator<char>(file), {});
}

OpenCLCompute::OpenCLCompute(std::vector<Particle> &aParticles) : mParticles(aParticles)
{
    // 1) Select platform and device
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty())
        throw std::runtime_error("No OpenCL platforms found");
    cl::Platform platform = platforms.front();

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    if (devices.empty())
        throw std::runtime_error("No GPU devices found on platform");
    cl::Device device = devices.front();

    // 2) Create context and command queue
    mContext = cl::Context(device);
    mQueue = cl::CommandQueue(mContext, device);

    // 3) Build program
    std::string source = loadKernelSource("updatePositions.cl");
    cl::Program::Sources sources{{source.c_str(), source.size()}};
    mProgram = cl::Program(mContext, sources);
    try
    {
        mProgram.build({device});
    }
    catch (...)
    {
        std::string buildLog = mProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
        std::cerr << "Build log:\n"
                  << buildLog << std::endl;
        throw;
    }

    // 4) Create kernel
    mKernelUpdatePositions = cl::Kernel(mProgram, "updatePositions");
}

void OpenCLCompute::updatePositionsGPU(float aDeltaTime)
{
    // 1) Prepare data
    const size_t particleCount = mParticles.size();
    std::vector<cl_float2> positions(particleCount);
    std::vector<cl_float2> velocities(particleCount);

    for (size_t i = 0; i < particleCount; ++i)
    {
        positions[i] = {mParticles[i].mPosition.x, mParticles[i].mPosition.y};
        velocities[i] = {mParticles[i].mVelocity.x, mParticles[i].mVelocity.y};
    }

    // 2) Create buffers
    cl::Buffer bufferPositions(mContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                sizeof(cl_float2) * particleCount, positions.data());
    cl::Buffer bufferVelocities(mContext, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(cl_float2) * particleCount, velocities.data());

    // 3) Set kernel arguments
    mKernelUpdatePositions.setArg(0, bufferPositions);
    mKernelUpdatePositions.setArg(1, bufferVelocities);
    mKernelUpdatePositions.setArg(2, aDeltaTime);

    // 4) Execute kernel
    cl::NDRange global(particleCount);
    cl_int err = mQueue.enqueueNDRangeKernel(mKernelUpdatePositions, cl::NullRange, global, cl::NullRange);
    if (err != CL_SUCCESS)
    {
        std::cerr << "Kernel launch failed: " << err << std::endl;
    }

    // 5) Read back results
    mQueue.enqueueReadBuffer(bufferPositions, CL_TRUE, 0,
                             sizeof(cl_float2) * particleCount, positions.data());


    // Update mParticles with new positions
    for (size_t i = 0; i < particleCount; ++i)
    {
        mParticles[i].mPosition.x = positions[i].x;
        mParticles[i].mPosition.y = positions[i].y;
    }
}