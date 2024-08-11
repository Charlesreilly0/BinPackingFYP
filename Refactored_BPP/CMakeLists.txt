cmake_minimum_required(VERSION 3.10)

project(BinPackingGame)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# find all source files in the src directory
file(GLOB SOURCES "src/*.cpp")

# Create a library from the core sources
add_library(BinPackingLib ${SOURCES})

# Add the main executable
add_executable(BinPackingGame src/main.cpp)

# Link the library to the main executable
target_link_libraries(BinPackingGame BinPackingLib)

# Add GoogleTest
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Add test sources
file(GLOB TEST_SOURCES "tests/*.cpp")

# Add the test executable
add_executable(runTests ${TEST_SOURCES})

# Link GTest and the core library to the test executable
target_link_libraries(runTests ${GTEST_LIBRARIES} pthread BinPackingLib)
add_test(NAME runTests COMMAND runTests)
