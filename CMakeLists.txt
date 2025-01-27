# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

cmake_minimum_required(VERSION 3.10)

# Set the project name
project(individualProjectAvans)


# set(CMAKE_AUTOMOC ON) # Enable automatic invocation of the Meta-Object Compiler (moc) 
set(CMAKE_AUTOUIC ON) # Enable automatic invocation of the User Interface Compiler (uic)
set(CMAKE_AUTORCC ON) # Enable automatic invocation of the Resource Compiler (rcc)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)
find_package(OpenCL REQUIRED)

# Add include directories
include_directories(${PROJECT_SOURCE_DIR}/code/inc)

# Add source directory
file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/code/src/*.cpp)

# Add the executable
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})

# Link the libraries
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets OpenCL::OpenCL)

add_definitions(-DCL_HPP_TARGET_OPENCL_VERSION=300)
