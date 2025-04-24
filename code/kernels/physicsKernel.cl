//#define M_PI 3.14159265358979323846f
#define MASS 1.0f

static float smoothingKernel(float smoothingRadius, float distance) {
    if (distance >= smoothingRadius) return 0.0f;
    float vol = (M_PI * smoothingRadius*smoothingRadius*smoothingRadius*smoothingRadius) * 0.16666667f; // πh^4/6
    float x = (smoothingRadius - distance);
    return (x*x) / vol;
}

static float smoothingKernelDerivative(float smoothingRadius, float distance) {
    if (distance >= smoothingRadius) return 0.0f;
    float scale = 12.0f / (M_PI * smoothingRadius*smoothingRadius*smoothingRadius*smoothingRadius);
    return (distance - smoothingRadius) * scale;
}

static float viscosityKernel(float smoothingRadius, float distance) {
    float vol = (M_PI * smoothingRadius*smoothingRadius*smoothingRadius*smoothingRadius*smoothingRadius*smoothingRadius*smoothingRadius*smoothingRadius) * 0.25f; // πh^8/4
    float val = fmax(0.0f, smoothingRadius*smoothingRadius - distance*distance);
    return (val*val*val) / vol;
}

__kernel void computeDensity(
    __global const float2* positions,
    __global float* densities,
    const int n,
    const float smoothingRadius)
{
    int i = get_global_id(0);
    float2 positionParticle = positions[i];
    float rho = 0.0f;
    for (int j = 0; j < n; ++j) {
        float2 positionOtherParticle = positions[j];
        float dx = positionOtherParticle.x - positionParticle.x;
        float dy = positionOtherParticle.y - positionParticle.y;
        float distance = sqrt(dx*dx + dy*dy);
        rho += MASS * smoothingKernel(smoothingRadius, distance);
    }
    densities[i] = rho;
}

__kernel void integrate(
    __global float2* positions,
    __global float2* velocities,
    __global const float* densities,
    const float dt,
    const float targetDensity,
    const float pressureMultiplier,
    const float viscosityMultiplier,
    const float gravity,
    const float smoothingRadius)
{
    int i = get_global_id(0);
    float2 positionParticle = positions[i];
    float2 velocityParticle = velocities[i];
    float densityParticle = densities[i];
    
    // Compute pressure
    float pressure_i = pressureMultiplier * (densityParticle - targetDensity);

    float2 accel = (float2)(0.0f, gravity);
    
    // Interaction forces
    for (int j = 0; j < get_global_size(0); ++j) {
        if (j == i) continue;
        float2 positionOtherParticle = positions[j];
        float dx = positionOtherParticle.x - positionParticle.x;
        float dy = positionOtherParticle.y - positionParticle.y;
        float distance = sqrt(dx*dx + dy*dy);
        if (distance <= 0.0f || distance > smoothingRadius) continue;

        // Pressure force
        float rho_j = densities[j];
        float pressure_j = pressureMultiplier * (rho_j - targetDensity);
        float sharedP = 0.5f * (pressure_i + pressure_j);
        float grad = smoothingKernelDerivative(smoothingRadius, distance);
        float2 dir = (float2)(dx/distance, dy/distance);
        accel -= dir * (sharedP / rho_j) * grad;

        // Viscosity
        float2 velocityOtherParticle = velocities[j];
        float visc = viscosityKernel(smoothingRadius, distance);
        accel += (velocityOtherParticle - velocityParticle) * (visc / densityParticle) * viscosityMultiplier;
    }

    // Integrate
    velocityParticle += accel * dt;
    positionParticle += velocityParticle * dt;

    velocities[i] = velocityParticle;
    positions[i]  = positionParticle;
}