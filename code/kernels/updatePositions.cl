__kernel void updatePositions(__global float2* positions,
                               __global const float2* velocities,
                               const float dt) {
    int i = get_global_id(0);
    positions[i] += velocities[i] * dt;
}