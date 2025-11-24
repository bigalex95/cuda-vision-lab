cmake_minimum_required(VERSION 3.18)
project(CuVO-Kernels LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_ARCHITECTURES "native") 

find_package(OpenCV REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/common)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Subdirectories (Uncomment as you progress)
# add_subdirectory(src/01_pixel_ops)
