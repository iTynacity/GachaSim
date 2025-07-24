# Specify the minimum version of CMake required
cmake_minimum_required(VERSION 3.16)

# Set the project name
project(GachaSimulator)

# Set the C++ standard to C++17 for modern features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the thread library, required for multithreading
find_package(Threads REQUIRED)

# Add the executable and link libraries
add_executable(GachaSimulator main.cpp)
target_link_libraries(GachaSimulator PRIVATE Threads::Threads)

# Recommend building in Release mode for performance
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configuration types" FORCE)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()