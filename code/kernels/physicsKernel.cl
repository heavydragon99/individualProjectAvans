#define MASS 1.0f
#define COLLISION_DAMPING 0.8f
#define MAX_NEIGHBORS 100

static float smoothingKernel(float aRadius, float aDistance)
{
    if (aDistance >= aRadius)
        return 0.0f;
    float vol = (M_PI_F * aRadius * aRadius * aRadius * aRadius) * 0.16666667f; // πh^4/6
    float x = (aRadius - aDistance);
    return (aRadius - aDistance) * (aRadius - aDistance) / vol;
}

static float smoothingKernelDerivative(float aRadius, float aDistance)
{
    if (aDistance >= aRadius)
        return 0.0f;
    float scale = 12.0f / (M_PI_F * aRadius * aRadius * aRadius * aRadius);
    return (aDistance - aRadius) * scale;
}

static float viscosityKernelLaplacian(float aRadius, float aDistance)
{
    if (aDistance >= aRadius)
        return 0.0f;
    float coeff = 45.0f / (M_PI_F * pow(aRadius, 6));
    return coeff * (aRadius - aDistance);
}

static float poly6Kernel(float aRadius, float aDistance)
{
    if (aDistance >= aRadius)
        return 0.0f;
    float x = aRadius * aRadius - aDistance * aDistance;
    float coeff = 315.0f / (64.0f * M_PI_F * pow(aRadius, 9));
    return coeff * x * x * x;
}

static float spikyKernelGradient(float aRadius, float aDistance)
{
    if (aDistance <= 0.0f || aDistance >= aRadius)
        return 0.0f;
    float coeff = -45.0f / (M_PI_F * pow(aRadius, 6));
    return coeff * (aRadius - aDistance) * (aRadius - aDistance);
}

__kernel void predictPositions(
    __global const float2 *aPositions,
    __global const float2 *aVelocities,
    __global float2 *aPredictedPositions)
{
    int particleIndex = get_global_id(0);
    float2 positionParticle = aPositions[particleIndex];
    float2 velocityParticle = aVelocities[particleIndex];
    aPredictedPositions[particleIndex] = positionParticle + velocityParticle * 0.016666667f; // position + velocity * 1/60
}

__kernel void computeDensity(
    __global const float2 *aPredictedPositions,
    __global float *aDensities,
    __global const int *aNeighbors,
    __global const int *aNeighborCounts,
    const int aParticleCount,
    const float aSmoothingRadius)
{
    int particleIndex = get_global_id(0);
    float2 positionParticle = aPredictedPositions[particleIndex];
    float density = 0.0f;
    int baseIdx = particleIndex * MAX_NEIGHBORS;
    int neighborCount = aNeighborCounts[particleIndex];
    for (int i = 0; i < neighborCount; ++i)
    {
        int otherparticleIndex = aNeighbors[baseIdx + i];
        float2 PredictedPositionOtherParticle = aPredictedPositions[otherparticleIndex];
        float dx = PredictedPositionOtherParticle.x - positionParticle.x;
        float dy = PredictedPositionOtherParticle.y - positionParticle.y;
        float distance = native_sqrt(dx * dx + dy * dy);
        density += MASS * poly6Kernel(aSmoothingRadius, distance);
    }
    aDensities[particleIndex] = density;
}

__kernel void integrate(
    __global float2 *aPositions,
    __global float2 *aVelocities,
    __global const float2 *aPredictedPositions,
    __global const float *aDensities,
    __global const int *aNeighbors,
    __global const int *aNeighborCounts,
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

    int baseIdx = particleIndex * MAX_NEIGHBORS;
    int neighborCount = aNeighborCounts[particleIndex];
    for (int i = 0; i < neighborCount; ++i)
    {
        int otherparticleIndex = aNeighbors[baseIdx + i];
        if (otherparticleIndex == particleIndex)
            continue;

        float2 PredictedPositionOtherParticle = aPredictedPositions[otherparticleIndex];
        float dx = PredictedPositionOtherParticle.x - predictedPositionParticle.x;
        float dy = PredictedPositionOtherParticle.y - predictedPositionParticle.y;
        float distance = native_sqrt(dx * dx + dy * dy);
        if (distance <= 0.0f || distance > aSmoothingRadius)
            continue;

        // Pressure force
        float densityOther = aDensities[otherparticleIndex];
        if (densityOther <= 0.0f)
            continue; // Skip if density is zero or negative.
        float pressureOther = aPressureMultiplier * (densityOther - aTargetDensity);
        float sharedPressure = 0.5f * (pressureParticle + pressureOther);
        float gradient = spikyKernelGradient(aSmoothingRadius, distance);
        float2 dir = (float2)(dx / distance, dy / distance);
        pressureForce += -sharedPressure * dir * gradient * MASS / densityOther;

        // Viscosity
        float2 velocityOtherParticle = aVelocities[otherparticleIndex];
        float influence = viscosityKernelLaplacian(aSmoothingRadius, distance);
        viscosityForce += (velocityOtherParticle - velocityParticle) * influence * aViscosityMultiplier;
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

// Morton-code computation
inline uint interleaveBits(uint x)
{
    x = (x | (x << 8)) & 0x00FF00FF;
    x = (x | (x << 4)) & 0x0F0F0F0F;
    x = (x | (x << 2)) & 0x33333333;
    x = (x | (x << 1)) & 0x55555555;
    return x;
}

__kernel void computeMorton(
    __global const float2 *aPredictedPositions,
    __global uint *aKeys,  // Morton codes
    __global int *aValues, // Particle indices
    const float aGridSize,
    const int aCount)
{
    int i = get_global_id(0);
    if (i >= aCount)
        return;

    float2 p = aPredictedPositions[i];

    // Compute grid cell coordinates
    uint xi = (uint)(p.x / aGridSize);
    uint yi = (uint)(p.y / aGridSize);

    // Compute Morton code
    uint mortonCode = interleaveBits(xi) | (interleaveBits(yi) << 1);

    // Output key-value pair
    aKeys[i] = mortonCode;
    aValues[i] = i;
}

// Build cell bounds (start/end indices of each Morton code cell)
__kernel void buildCellBounds(
    __global const uint *aMortonCodes,
    __global int *aCellStart,
    __global int *aCellEnd,
    const int aCount)
{
    int i = get_global_id(0);
    if (i >= aCount)
        return;

    uint code = aMortonCodes[i];
    if (i == 0 || code != aMortonCodes[i - 1])
    {
        aCellStart[code] = i;
        if (i > 0)
            aCellEnd[aMortonCodes[i - 1]] = i;
    }
    if (i == aCount - 1)
    {
        aCellEnd[code] = aCount;
    }
}

__kernel void bitonicSort(
    __global uint *aKeys,  // Morton codes
    __global int *aValues, // Particle indices
    const uint aStage,
    const uint aPassOfStage)
{
    int id = get_global_id(0);
    uint pairDistance = 1 << (aStage - aPassOfStage);
    uint blockWidth = 2 * pairDistance;
    uint temp;

    uint leftId = (id / pairDistance) * blockWidth + (id % pairDistance);
    uint rightId = leftId + pairDistance;

    bool sameDirection = ((id / blockWidth) % 2 == 0);
    bool shouldSwap = (aKeys[leftId] > aKeys[rightId]) == sameDirection;

    if (shouldSwap)
    {
        temp = aKeys[leftId];
        aKeys[leftId] = aKeys[rightId];
        aKeys[rightId] = temp;

        int tempVal = aValues[leftId];
        aValues[leftId] = aValues[rightId];
        aValues[rightId] = tempVal;
    }
}

__kernel void reorderParticles(
    __global const float2 *aInput,
    __global float2 *aOutput,
    __global const int *aSortedIndices)
{
    int i = get_global_id(0);
    aOutput[i] = aInput[aSortedIndices[i]];
}

// Neighbor search using cell bounds + morton codes
__kernel void neighborSearch(
    __global const float2 *aPredictedPositions,
    __global const uint *aMortonCodes,
    __global const int *aCellStart,
    __global const int *aCellEnd,
    __global int *aNeighbors,
    __global int *aNeighborCounts,
    const float aGridSize,
    const float aSearchRadius,
    const int aCount)
{
    int i = get_global_id(0);
    if (i >= aCount)
        return;

    float2 p = aPredictedPositions[i];
    uint xi = (uint)(p.x / aGridSize);
    uint yi = (uint)(p.y / aGridSize);

    int baseOut = i * MAX_NEIGHBORS; // reserve up to MAX_NEIGHBORS neighbors
    int count = 0;

    for (int dx = -1; dx <= 1; ++dx)
    {
        for (int dy = -1; dy <= 1; ++dy)
        {
            uint nx = xi + dx, ny = yi + dy;
            uint cellCode = interleaveBits(nx) | (interleaveBits(ny) << 1);
            int start = aCellStart[cellCode];
            int end = aCellEnd[cellCode];
            for (int idx = start; idx < end; ++idx)
            {
                if (idx == i)
                    continue;
                float2 q = aPredictedPositions[idx];
                float dx = q.x - p.x, dy = q.y - p.y;
                float dist2 = dx * dx + dy * dy;
                if (dist2 <= aSearchRadius * aSearchRadius && count < MAX_NEIGHBORS)
                {
                    aNeighbors[baseOut + count] = idx;
                    ++count;
                }
            }
        }
    }
    aNeighborCounts[i] = count;
}