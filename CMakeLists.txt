# Minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(MyProject LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable target
add_executable(sand falling_sand.cpp)

# Link SFML libraries to the executable
target_link_libraries(sand sfml-graphics sfml-window sfml-system)