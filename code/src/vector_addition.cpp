#include <CL/opencl.hpp>
#include <vector>
#include <iostream>

int main() {
    // Step 1: Discover and select a platform
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if (platforms.empty()) {
        std::cerr << "No OpenCL platforms found!" << std::endl;
        return -1;
    }
    cl::Platform platform = platforms[0];

    // Step 2: Discover and select a GPU device
    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_GPU, &devices);
    if (devices.empty()) {
        std::cerr << "No GPU devices found!" << std::endl;
        return -1;
    }
    cl::Device device = devices[0];

    // Step 3: Create a context and command queue
    cl::Context context(device);
    cl::CommandQueue queue(context, device);

    // Step 4: Write and compile the kernel
    const char* kernelSource = R"CLC(
        __kernel void vector_add(__global const float* A, __global const float* B, __global float* C) {
            int id = get_global_id(0);
            C[id] = A[id] + B[id];
        }
    )CLC";
    cl::Program program(context, kernelSource);
    program.build({device});

    // Step 5: Create buffers and transfer data
    const int size = 1024;
    std::vector<float> A(size, 1.0f), B(size, 2.0f), C(size);
    cl::Buffer bufferA(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * size, A.data());
    cl::Buffer bufferB(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * size, B.data());
    cl::Buffer bufferC(context, CL_MEM_WRITE_ONLY, sizeof(float) * size);

    // Step 6: Set kernel arguments and execute
    cl::Kernel kernel(program, "vector_add");
    kernel.setArg(0, bufferA);
    kernel.setArg(1, bufferB);
    kernel.setArg(2, bufferC);
    queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(size));

    // Step 7: Retrieve results
    queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, sizeof(float) * size, C.data());

    // Verify results
    for (int i = 0; i < size; ++i) {
        if (C[i] != 3.0f) {
            std::cerr << "Error at index " << i << ": " << C[i] << std::endl;
            return -1;
        }
    }
    std::cout << "Vector addition successful!" << std::endl;

    return 0;
}
