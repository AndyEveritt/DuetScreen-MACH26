# Unit tests for DuetScreen using GoogleTest

# Find GoogleTest
find_package(GTest REQUIRED)

enable_testing()

add_executable(DuetScreen.tests)

# Add clang-format custom target
add_custom_target(
  clang-format-tests
  COMMAND find tests/ -path 'src/lv_i18n' -prune -o -iname '*.h' -o -iname '*.cpp'
  -print | xargs ${CLANG_FORMAT_19} -i
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Running clang-format-19-tests")
add_dependencies(DuetScreen.tests clang-format-tests)

file(GLOB_RECURSE DUETSCREEN_TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
target_sources(DuetScreen.tests PRIVATE ${DUETSCREEN_TEST_SOURCES})

target_include_directories(DuetScreen.tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_definitions(lvgl PUBLIC
    LV_USE_TEST=1
    LV_USE_TEST_SCREENSHOT_COMPARE=1
    LV_BUILD_TEST=1
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
# Ensure tests run from the project root so any relative paths in tests behave as expected
gtest_discover_tests(DuetScreen.tests
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    PROPERTIES TIMEOUT 30
)
