#include <iostream>
#include <vector>

// OpenCL includes
#include "opencl.hpp"

// int main()
// {
//     cl_int CL_err = CL_SUCCESS;
//     cl_uint numPlatforms = 0;

//     CL_err = clGetPlatformIDs(0, NULL, &numPlatforms);

//     if (CL_err == CL_SUCCESS)
//         printf("%u platform(s) found\n", numPlatforms);
//     else
//         printf("clGetPlatformIDs(%i)\n", CL_err);

//     return 0;
// }

int main()
{
    // get all platforms (drivers)
    std::vector<cl::Platform> all_platforms;
    cl::Platform::get(&all_platforms);
    if (all_platforms.size() == 0)
    {
        std::cout << " No platforms found. Check OpenCL installation!\n";
        exit(1);
    }
    cl::Platform default_platform = all_platforms[0];
    std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

    return 0;
}