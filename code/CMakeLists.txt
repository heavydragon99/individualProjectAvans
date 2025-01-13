cmake_minimum_required(VERSION 3.10)

# Set the project name
project(IndividualProjectAvans)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCL REQUIRED)
# Add include directories
include_directories(${PROJECT_SOURCE_DIR}/inc)

# Add source directory
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/src/*.cpp)
# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the OpenCL library
target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL)

# Set the build folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)