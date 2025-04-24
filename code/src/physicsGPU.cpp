// physics_opencl.cpp

#include "physics.h"
#include "simulationConfig.h"

#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#include <CL/cl2.hpp>

#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

class PhysicsGPU
{
public:
    PhysicsGPU(std::vector<Particle> &particles)
        : mParticles(particles), mContext(), mQueue(), mProgram() {}

    void initialize();
    void update(float deltaTime);

private:
    std::vector<Particle> &mParticles;
    cl::Context mContext;
    cl::CommandQueue mQueue;
    cl::Program mProgram;

    cl::Buffer mBufferPositions;
    cl::Buffer mBufferVelocities;
    cl::Buffer mBufferPredictedPositions;
    cl::Buffer mBufferDensities;
    cl::Buffer mBufferForces;

    void loadKernels();
};

const char *kernelSource = R"CLC(
__kernel void predictPositions(__global const float2* positions,
                               __global const float2* velocities,
                               __global float2* predictedPositions,
                               float deltaTime) {
    int id = get_global_id(0);
    predictedPositions[id] = positions[id] + velocities[id] * deltaTime;
}

__kernel void applyExternalForces(__global float2* velocities,
                                  __global const float2* positions,
                                  float gravity,
                                  float deltaTime,
                                  int isMousePressed,
                                  float2 mousePos,
                                  float forceMagnitude,
                                  int radius,
                                  int width, int height) {
    int id = get_global_id(0);
    float2 pos = positions[id];
    float2 vel = velocities[id];
    float2 interactionForce = (float2)(0.0f, 0.0f);

    if (isMousePressed) {
        float2 offset = mousePos - pos;
        float dist = length(offset);
        if (dist < radius) {
            float edgeT = dist / radius;
            float centreT = 1.0f - edgeT;
            float2 dirToCentre = normalize(offset);
            float gravityWeight = 1.0f - centreT;
            interactionForce = (float2)(0.0f, gravity) * gravityWeight + dirToCentre * centreT * forceMagnitude;
            interactionForce -= vel * centreT;
        }
    }

    float2 gravityForce = (float2)(0.0f, gravity);
    velocities[id] += (gravityForce + interactionForce) * deltaTime;
}

__kernel void updatePositions(__global float2* positions,
                              __global const float2* velocities,
                              float deltaTime) {
    int id = get_global_id(0);
    positions[id] += velocities[id] * deltaTime;
}

__kernel void checkBoundaries(__global float2* positions,
                              __global float2* velocities,
                              float radius,
                              float damping,
                              int width, int height) {
    int id = get_global_id(0);
    float2 pos = positions[id];
    float2 vel = velocities[id];

    if (pos.x < radius) {
        pos.x = radius;
        vel.x = -vel.x * damping;
    } else if (pos.x > width - radius) {
        pos.x = width - radius;
        vel.x = -vel.x * damping;
    }

    if (pos.y < radius) {
        pos.y = radius;
        vel.y = -vel.y * damping;
    } else if (pos.y > height - radius) {
        pos.y = height - radius;
        vel.y = -vel.y * damping;
    }

    positions[id] = pos;
    velocities[id] = vel;
}
)CLC";

void PhysicsGPU::initialize()
{
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    assert(!platforms.empty());

    cl_context_properties properties[] = {
        CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};
    mContext = cl::Context(CL_DEVICE_TYPE_GPU, properties);

    std::vector<cl::Device> devices = mContext.getInfo<CL_CONTEXT_DEVICES>();
    mQueue = cl::CommandQueue(mContext, devices[0]);

    loadKernels();

    size_t particleCount = mParticles.size();
    std::vector<cl_float2> positions(particleCount);
    std::vector<cl_float2> velocities(particleCount);
    for (size_t i = 0; i < particleCount; ++i)
    {
        positions[i] = {mParticles[i].mPosition.x, mParticles[i].mPosition.y};
        velocities[i] = {mParticles[i].mVelocity.x, mParticles[i].mVelocity.y};
    }

    mBufferPositions = cl::Buffer(mContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_float2) * particleCount, positions.data());
    mBufferVelocities = cl::Buffer(mContext, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_float2) * particleCount, velocities.data());
    mBufferPredictedPositions = cl::Buffer(mContext, CL_MEM_READ_WRITE, sizeof(cl_float2) * particleCount);
}

void PhysicsGPU::update(float deltaTime)
{
    int particleCount = static_cast<int>(mParticles.size());
    auto &config = SimulationConfig::getInstance();

    cl::Kernel predictKernel(mProgram, "predictPositions");
    predictKernel.setArg(0, mBufferPositions);
    predictKernel.setArg(1, mBufferVelocities);
    predictKernel.setArg(2, mBufferPredictedPositions);
    predictKernel.setArg(3, deltaTime);
    mQueue.enqueueNDRangeKernel(predictKernel, cl::NullRange, cl::NDRange(particleCount));

    cl::Kernel forceKernel(mProgram, "applyExternalForces");
    forceKernel.setArg(0, mBufferVelocities);
    forceKernel.setArg(1, mBufferPositions);
    forceKernel.setArg(2, config.gravity());
    forceKernel.setArg(3, deltaTime);
    forceKernel.setArg(4, (int)(config.isMousePressedLeft() || config.isMousePressedRight()));
    sf::Vector2f mouse = config.mousePosition();
    forceKernel.setArg(5, cl_float2{mouse.x, mouse.y});
    forceKernel.setArg(6, config.isMousePressedLeft() ? -40.0f : 10.0f);
    forceKernel.setArg(7, std::min(config.gameSize().x, config.gameSize().y) * 0.1f);
    forceKernel.setArg(8, config.gameSize().x);
    forceKernel.setArg(9, config.gameSize().y);
    mQueue.enqueueNDRangeKernel(forceKernel, cl::NullRange, cl::NDRange(particleCount));

    cl::Kernel updatePosKernel(mProgram, "updatePositions");
    updatePosKernel.setArg(0, mBufferPositions);
    updatePosKernel.setArg(1, mBufferVelocities);
    updatePosKernel.setArg(2, deltaTime);
    mQueue.enqueueNDRangeKernel(updatePosKernel, cl::NullRange, cl::NDRange(particleCount));

    cl::Kernel boundaryKernel(mProgram, "checkBoundaries");
    boundaryKernel.setArg(0, mBufferPositions);
    boundaryKernel.setArg(1, mBufferVelocities);
    boundaryKernel.setArg(2, mParticles[0].mRadius);
    boundaryKernel.setArg(3, COLLISION_DAMPING);
    boundaryKernel.setArg(4, config.gameSize().x);
    boundaryKernel.setArg(5, config.gameSize().y);
    mQueue.enqueueNDRangeKernel(boundaryKernel, cl::NullRange, cl::NDRange(particleCount));

    mQueue.finish();

    std::vector<cl_float2> positions(particleCount);
    std::vector<cl_float2> velocities(particleCount);
    mQueue.enqueueReadBuffer(mBufferPositions, CL_TRUE, 0, sizeof(cl_float2) * particleCount, positions.data());
    mQueue.enqueueReadBuffer(mBufferVelocities, CL_TRUE, 0, sizeof(cl_float2) * particleCount, velocities.data());

    for (int i = 0; i < particleCount; ++i)
    {
        mParticles[i].mPosition = {positions[i].x, positions[i].y};
        mParticles[i].mVelocity = {velocities[i].x, velocities[i].y};
    }
}

void PhysicsGPU::loadKernels()
{
    cl::Program::Sources sources;
    sources.push_back({kernelSource, strlen(kernelSource)});
    mProgram = cl::Program(mContext, sources);
    if (mProgram.build({mContext.getInfo<CL_CONTEXT_DEVICES>()[0]}) != CL_SUCCESS)
    {
        std::cerr << "Error building OpenCL program: " << mProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(mContext.getInfo<CL_CONTEXT_DEVICES>()[0]) << std::endl;
        exit(1);
    }
}
