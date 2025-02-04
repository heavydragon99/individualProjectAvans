# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

cmake_minimum_required(VERSION 3.10)

# Set the project name
project(individualProjectAvans)

set(CMAKE_AUTOMOC ON) # Enable automatic invocation of the Meta-Object Compiler (moc) 
set(CMAKE_AUTOUIC ON) # Enable automatic invocation of the User Interface Compiler (uic)
set(CMAKE_AUTORCC ON) # Enable automatic invocation of the Resource Compiler (rcc)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directory for the library
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Specify the C# project
set(CS_PROJECT ${CMAKE_SOURCE_DIR}/code/c\#/individualProject.csproj)

# Add a custom target to build the C# project using dotnet
add_custom_target(CSharpApp ALL
    COMMAND dotnet build -o ${CMAKE_BINARY_DIR} ${CS_PROJECT}
    DEPENDS PhysicsLibrary
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Building C# application"
)

# Add subdirectory for C++ code
add_subdirectory(code/c++)