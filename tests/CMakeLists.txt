# Add include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

# Create test executable
add_executable(
    unit_tests
    OrderTests.cpp
    OrderBookTests.cpp
    OrderFactoryTests.cpp
    MatchingEngineTests.cpp
    TradeTests.cpp
    ContinuousMatchingEngineTests.cpp
    ThreadingTests.cpp
)

# Link with our library and Google Test
target_link_libraries(
    unit_tests
    matching_engine_lib
    gtest
    gtest_main
)

# Add tests to CTest
add_test(NAME UnitTests COMMAND unit_tests)
