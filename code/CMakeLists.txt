cmake_minimum_required(VERSION 3.10)

# Set the project name
project(individualProjectAvans)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCL REQUIRED)

# Add git submodule
add_subdirectory(${PROJECT_SOURCE_DIR}/modules/SFML)

# Add include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/inc 
    ${PROJECT_SOURCE_DIR}/modules/SFML/include
)

# Add source directory
set(SOURCES
    ${PROJECT_SOURCE_DIR}/src/fluidSimulation.cpp
    ${PROJECT_SOURCE_DIR}/src/main.cpp
    ${PROJECT_SOURCE_DIR}/src/particleSystem.cpp
    ${PROJECT_SOURCE_DIR}/src/physics.cpp
    ${PROJECT_SOURCE_DIR}/src/renderer.cpp
    ${PROJECT_SOURCE_DIR}/modules/imgui/imgui_demo.cpp
    ${PROJECT_SOURCE_DIR}/modules/imgui/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/modules/imgui/imgui_tables.cpp
    ${PROJECT_SOURCE_DIR}/modules/imgui/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/modules/imgui/imgui.cpp
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the libraries
target_link_libraries(${PROJECT_NAME} PRIVATE OpenCL::OpenCL sfml-system sfml-network sfml-graphics sfml-window)

add_definitions(-DCL_HPP_TARGET_OPENCL_VERSION=300)