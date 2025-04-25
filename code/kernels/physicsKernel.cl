#define MASS 1.0f
#define COLLISION_DAMPING 0.8f

static float smoothingKernel(float aRadius, float aDistance)
{
    if (aDistance >= aRadius)
        return 0.0f;
    float vol = (M_PI * aRadius * aRadius * aRadius * aRadius) * 0.16666667f; // πh^4/6
    float x = (aRadius - aDistance);
    return (x * x) / vol;
}

static float smoothingKernelDerivative(float aRadius, float aDistance)
{
    if (aDistance >= aRadius)
        return 0.0f;
    float scale = 12.0f / (M_PI * aRadius * aRadius * aRadius * aRadius);
    return (aDistance - aRadius) * scale;
}

static float viscosityKernel(float aRadius, float aDistance)
{
    float vol = (M_PI * aRadius * aRadius * aRadius * aRadius * aRadius * aRadius * aRadius * aRadius) * 0.25f; // πh^8/4
    float val = fmax(0.0f, aRadius * aRadius - aDistance * aDistance);
    return (val * val * val) / vol;
}

__kernel void predictPositions(
    __global const float2 *aPositions,
    __global const float2 *aVelocities,
    __global float2 *aPredictedPositions)
{
    int particleIndex = get_global_id(0);
    float2 positionParticle = aPositions[particleIndex];
    float2 velocityParticle = aVelocities[particleIndex];
    aPredictedPositions[particleIndex] = positionParticle + velocityParticle * 0.16666667f; // position + velocity * 1/60
}

__kernel void computeDensity(
    __global const float2 *aPredictedPositions,
    __global float *aDensities,
    const int aParticleCount,
    const float aSmoothingRadius)
{
    int particleIndex = get_global_id(0);
    float2 positionParticle = aPredictedPositions[particleIndex];
    float density = 0.0f;
    for (int otherparticleIndex = 0; otherparticleIndex < aParticleCount; ++otherparticleIndex)
    {
        float2 PredictedPositionOtherParticle = aPredictedPositions[otherparticleIndex];
        float dx = PredictedPositionOtherParticle.x - positionParticle.x;
        float dy = PredictedPositionOtherParticle.y - positionParticle.y;
        float distance = sqrt(dx * dx + dy * dy);
        density += MASS * smoothingKernel(aSmoothingRadius, distance);
    }
    aDensities[particleIndex] = density;
}

__kernel void integrate(
    __global float2 *aPositions,
    __global float2 *aVelocities,
    __global const float2 *aPredictedPositions,
    __global const float *aDensities,
    const float aDeltaTime,
    const float aTargetDensity,
    const float aPressureMultiplier,
    const float aViscosityMultiplier,
    const float aGravity,
    const float aSmoothingRadius,
    const float aParticleRadius,
    const float aGameHeight,
    const float aGameWidth)
{
    int particleIndex = get_global_id(0);
    float2 positionParticle = aPositions[particleIndex];
    float2 velocityParticle = aVelocities[particleIndex];
    float densityParticle = aDensities[particleIndex];
    float2 predictedPositionParticle = aPredictedPositions[particleIndex];

    // Compute pressure
    float pressureParticle = aPressureMultiplier * (densityParticle - aTargetDensity);

    float2 accel = (float2)(0.0f, 0.0f);
    float2 pressureForce = (float2)(0.0f, 0.0f);
    float2 viscosityForce = (float2)(0.0f, 0.0f);

    // Interaction forces
    for (int otherparticleIndex = 0; otherparticleIndex < get_global_size(0); ++otherparticleIndex)
    {
        if (otherparticleIndex == particleIndex)
            continue;
        float2 PredictedPositionOtherParticle = aPredictedPositions[otherparticleIndex];
        float dx = PredictedPositionOtherParticle.x - predictedPositionParticle.x;
        float dy = PredictedPositionOtherParticle.y - predictedPositionParticle.y;
        float distance = sqrt(dx * dx + dy * dy);
        if (distance <= 0.0f || distance > aSmoothingRadius)
            continue;

        // Pressure force
        float densityOther = aDensities[otherparticleIndex];
        if (densityOther <= 0.0f)
            continue; // Skip if density is zero or negative.
        float pressureOther = aPressureMultiplier * (densityOther - aTargetDensity);
        float sharedPressure = 0.5f * (pressureParticle + pressureOther);
        float gradient = smoothingKernelDerivative(aSmoothingRadius, distance);
        float2 dir = (float2)(dx / distance, dy / distance);
        pressureForce -= sharedPressure * dir * gradient * MASS / densityOther;

        // Viscosity
        // float2 velocityOtherParticle = aVelocities[otherparticleIndex];
        // float influence = viscosityKernel(aSmoothingRadius, distance);
        // viscosityForce += (velocityOtherParticle - velocityParticle) * influence * aViscosityMultiplier;
    }


    float2 totalPressure = (pressureForce + viscosityForce) / densityParticle;
    totalPressure += (float2)(0.0f, aGravity);

    // Integrate
    velocityParticle += totalPressure * aDeltaTime;
    positionParticle += velocityParticle * aDeltaTime;

    // Boundary conditions
    if (positionParticle.x < aParticleRadius)
    {
        positionParticle.x = aParticleRadius;
        velocityParticle.x *= -COLLISION_DAMPING;
    }
    else if (positionParticle.x > aGameWidth - aParticleRadius)
    {
        positionParticle.x = aGameWidth - aParticleRadius;
        velocityParticle.x *= -COLLISION_DAMPING;
    }
    if (positionParticle.y < aParticleRadius)
    {
        positionParticle.y = aParticleRadius;
        velocityParticle.y *= -COLLISION_DAMPING;
    }
    else if (positionParticle.y > aGameHeight - aParticleRadius)
    {
        positionParticle.y = aGameHeight - aParticleRadius;
        velocityParticle.y *= -COLLISION_DAMPING;
    }


    // Update positions and velocities
    aVelocities[particleIndex] = velocityParticle;
    aPositions[particleIndex] = positionParticle;
}