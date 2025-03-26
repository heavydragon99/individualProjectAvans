cmake_minimum_required(VERSION 3.10)

# Set the project name
project(individualProjectAvans)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCL REQUIRED)
find_package(SFML 3.0 COMPONENTS Graphics Window System REQUIRED)

# Add include directories
include_directories(${PROJECT_SOURCE_DIR}/inc)

# Add source directory
set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/fluidSimulation.cpp
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/particleSystem.cpp
    ${PROJECT_SOURCE_DIR}/src/physics.cpp
    ${PROJECT_SOURCE_DIR}/src/renderer.cpp
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the libraries
target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL sfml-graphics sfml-window sfml-system)

add_definitions(-DCL_HPP_TARGET_OPENCL_VERSION=300)