# Unit tests for DuetScreen using GoogleTest

# Find GoogleTest
find_package(GTest REQUIRED)

enable_testing()

add_executable(DuetScreen.tests)

file(GLOB_RECURSE DUETSCREEN_TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
target_sources(DuetScreen.tests PRIVATE ${DUETSCREEN_TEST_SOURCES})

# Dependencies
target_link_libraries(DuetScreen.tests PRIVATE
    GTest::gtest_main
    DuetScreen.lib
)

include(GoogleTest)
gtest_discover_tests(DuetScreen.tests)
