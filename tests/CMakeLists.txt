# Unit tests for DuetScreen using GoogleTest

# Find GoogleTest
find_package(GTest REQUIRED)

enable_testing()

add_executable(DuetScreen.tests)

file(GLOB_RECURSE DUETSCREEN_TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
target_sources(DuetScreen.tests PRIVATE ${DUETSCREEN_TEST_SOURCES})

target_include_directories(DuetScreen.tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(lvgl PUBLIC
    LV_USE_TEST=1
    LV_USE_TEST_SCREENSHOT_COMPARE=1
    REF_IMGS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/ref_imgs/"
)

# Dependencies
target_link_libraries(DuetScreen.tests PRIVATE
    GTest::gtest_main
)

# Link the executable with the DuetScreen core library.
# There is some static initialisation in some translation units so force linker to include everything.
target_link_libraries(DuetScreen.tests PRIVATE
    -Wl,--whole-archive DuetScreen.lib -Wl,--no-whole-archive
)

define_file_relpath_for_sources(DuetScreen.tests)

include(GoogleTest)
gtest_discover_tests(DuetScreen.tests)
