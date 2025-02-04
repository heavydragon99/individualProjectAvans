project(PhysicsLibrary)

find_package(OpenCL REQUIRED)

# Add include directories
include_directories(${PROJECT_SOURCE_DIR}/inc)

# Add source directory
set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/Physics.cpp
)

# Create the shared library
add_library(${PROJECT_NAME} SHARED ${SOURCES})

# Link the libraries
target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)

add_definitions(-DCL_HPP_TARGET_OPENCL_VERSION=300)