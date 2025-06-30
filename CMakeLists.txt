# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(QuantPricer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(include)

# === SOURCES ===
file(GLOB_RECURSE SRC_FILES src/*.cpp)

# === Main Executable ===
add_executable(pricer ${SRC_FILES} main.cpp)

