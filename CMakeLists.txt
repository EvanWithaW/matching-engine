cmake_minimum_required(VERSION 3.31)
project(matching_engine)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Add threading subdirectory
add_subdirectory(threading)

# Define library sources (excluding main.cpp)
set(LIB_SOURCES
    order/Order.cpp
    order/OrderBook.cpp
    order/OrderFactory.cpp
    order/Comparators.cpp
    engine/MatchingEngine.cpp
    engine/Trade.cpp
    engine/ContinuousMatchingEngine.cpp
)

# Create a library with the common code
add_library(matching_engine_lib STATIC ${LIB_SOURCES})

# Link the threading library
target_link_libraries(matching_engine_lib threading)

# Create the main executable
add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} matching_engine_lib)

# Enable testing
enable_testing()

# Option to build tests (ON by default)
option(BUILD_TESTS "Build the tests" ON)

if(BUILD_TESTS)
    # Include FetchContent for downloading GoogleTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    # Add tests directory
    add_subdirectory(tests)
endif()