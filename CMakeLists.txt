cmake_minimum_required(VERSION 3.10)
project(DuetScreen C CXX)
include(FetchContent)
include(CTest) # Enables BUILD_TESTING and CTest integration

# Set C and C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
set(LIBRARIES_DIR ${PROJECT_SOURCE_DIR}/libraries)
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Remove ARM-specific compile and linker options
if(PROCESSOR STREQUAL "Simulation")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

# Set optimization level based on build type
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -g")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
endif()

set(MAIN_SOURCES "")

# Options
set(PROCESSOR "T113" CACHE STRING "Target processor, 'T113' or 'Simulation'")
option(DEBUG_BORDERS "Enable debug borders" OFF)
option(LOG_TIMESTAMPS "Enable timestamps in log messages" ON)
option(LOG_THREAD "Enable logging thread id in log messages" OFF)
option(ASYNC_RR_MODEL "Enable asynchronous rr_model requests" OFF)
option(HARDWARE_TEST "Enable hardware test" OFF)
option(BURNIN_TEST "Enable burnin test" OFF)
option(ENABLE_CONSOLE_SHELL "Enable sending shell commands via the GUI console" OFF)
option(LV_NESTED_SHOW_HIDE "Enable nested show/hide events for LvObj" OFF)

if(NOT DEFINED PROCESSOR)
  message(
    FATAL_ERROR
      "PROCESSOR variable is not defined. Please set it to 'T113' or 'Simulation'."
  )
endif()

# ----------------------------------------------------------------------------
# Set source files for executable
# ----------------------------------------------------------------------------
file(GLOB_RECURSE DUET_LIB_SOURCES ${LIBRARIES_DIR}/Duet3D/**.c
${LIBRARIES_DIR}/Duet3D/**.cpp)

list(APPEND MAIN_SOURCES ${DUET_LIB_SOURCES})

add_library(DuetScreen.lib ${MAIN_SOURCES})
add_executable(DuetScreen)

# Find clang-format-19
find_program(CLANG_FORMAT_19 clang-format-19 REQUIRED)

# Add clang-format custom target
add_custom_target(
  clang-format
  COMMAND find src/ -iname '*.h' -o -iname '*.cpp'
  -print | xargs ${CLANG_FORMAT_19} -i
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Running clang-format-19")
add_dependencies(DuetScreen.lib clang-format)
  
  # Touch BuildDate.cpp to ensure it is always compiled
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
  add_custom_command(
    TARGET DuetScreen
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E touch ${SRC_DIR}/BuildDate.cpp
    COMMENT "Touching BuildDate.cpp to force recompilation")
    endif()
    
target_include_directories(DuetScreen.lib PUBLIC ${INCLUDE_DIR})
target_include_directories(DuetScreen.lib PUBLIC ${SRC_DIR})
target_include_directories(DuetScreen.lib PUBLIC ${LIBRARIES_DIR})

add_subdirectory(src)

# ----------------------------------------------------------------------------
# Libraries
# ----------------------------------------------------------------------------

# Color manipulation library
include (${LIBRARIES_DIR}/colorm.cmake)

# option(BUILD_SHARED_LIBS "Set default library type to shared" OFF)

# LVGL
include(${LIBRARIES_DIR}/lvgl.cmake)

# Networking
include(${LIBRARIES_DIR}/hv.cmake)

# Reflection
include(${LIBRARIES_DIR}/nameof.cmake)

# Find libusb package
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

# Find and include SDL2 library
find_package(SDL2 REQUIRED)

# Find json library
include(${LIBRARIES_DIR}/json.cmake)

# Logging
include(${LIBRARIES_DIR}/spdlog.cmake)

# Include libusb headers
target_include_directories(DuetScreen.lib PUBLIC ${LIBUSB_INCLUDE_DIRS})

# ----------------------------------------------------------------------------
# Add Definitions
# ----------------------------------------------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(DuetScreen.lib PUBLIC DEBUG)
else()
  set_target_properties(DuetScreen.lib PROPERTIES CMAKE_INTERPROCEDURAL_OPTIMIZATION
                                              TRUE)
endif()
target_compile_definitions(DuetScreen.lib PUBLIC MULTITHREADED)

if(PROCESSOR STREQUAL "T113")
  target_compile_definitions(lvgl PUBLIC 
                                  LV_USE_LINUX_FBDEV=1
                                  LV_USE_EVDEV=1
                                  LV_LINUX_FBDEV_RENDER_MODE=LV_DISPLAY_RENDER_MODE_DIRECT
                                  LV_LINUX_FBDEV_BUFFER_COUNT=2)
  target_compile_definitions(DuetScreen.lib PUBLIC T113=1)
  # add_compile_definitions(T113=1)
elseif(PROCESSOR STREQUAL "Simulation")
  target_compile_definitions(lvgl PUBLIC 
                                  LV_USE_SDL=1
                                  LV_SDL_BUF_COUNT=2
                                  LV_SDL_RENDER_MODE=LV_DISPLAY_RENDER_MODE_DIRECT)
  target_compile_definitions(DuetScreen.lib PUBLIC SIMULATION=1)
endif()

if(DEBUG_BORDERS)
  target_compile_definitions(DuetScreen.lib PUBLIC DEBUG_BORDERS=1)
endif()

if(LOG_TIMESTAMPS)
  target_compile_definitions(DuetScreen.lib PUBLIC LOG_TIMESTAMPS=1)
endif()

if(LOG_THREAD)
  target_compile_definitions(DuetScreen.lib PUBLIC LOG_THREAD=1)
endif()

if(ASYNC_RR_MODEL)
  target_compile_definitions(DuetScreen.lib PUBLIC ASYNC_RR_MODEL=1)
endif()

if(HARDWARE_TEST)
  target_compile_definitions(DuetScreen.lib PUBLIC HARDWARE_TEST=1)
endif()

if (BURNIN_TEST)
  target_compile_definitions(DuetScreen.lib PUBLIC BURNIN_TEST=1)
endif()

if (ENABLE_CONSOLE_SHELL)
  target_compile_definitions(DuetScreen.lib PUBLIC ENABLE_CONSOLE_SHELL=1)
endif()

if (LV_NESTED_SHOW_HIDE)
  target_compile_definitions(DuetScreen.lib PUBLIC LV_NESTED_SHOW_HIDE=1)
endif()

# Helper function to add preprocesor definition of FILE_BASENAME to pass the
# filename without directory path for debugging use.
#
# Note that in header files this is not consistent with __FILE__ and __LINE__
# since FILE_BASENAME will be the compilation unit source file name (.c/.cpp).
#
# Example:
#
# define_file_basename_for_sources(my_target)
#
# Will add -DFILE_BASENAME="filename" for each source file depended on by
# my_target, where filename is the name of the file.
#
function(define_file_relpath_for_sources targetname)
  get_target_property(source_files "${targetname}" SOURCES)
  foreach(sourcefile ${source_files})
    # Add the FILE_BASENAME=filename compile definition to the list.
    file(RELATIVE_PATH relpath "${PROJECT_SOURCE_DIR}" "${sourcefile}")
    # Set the updated compile definitions on the source file.
    set_property(
      SOURCE "${sourcefile}"
      APPEND
      PROPERTY COMPILE_DEFINITIONS "__FILE_RELPATH__=\"${relpath}\"")
  endforeach()
endfunction()

define_file_relpath_for_sources(DuetScreen.lib)
define_file_relpath_for_sources(DuetScreen)

# Define LVGL configuration as a simple include
target_compile_definitions(DuetScreen.lib PUBLIC LV_CONF_INCLUDE_SIMPLE)
target_link_libraries(
  DuetScreen.lib
  PUBLIC lvgl
          ${SDL2_LIBRARIES}
          ${LIBUSB_LIBRARIES} # Link libusb library
          m
          pthread
          hv_static
          nlohmann_json::nlohmann_json
          nameof::nameof
          spdlog::spdlog
          fmt::fmt)

if(PROCESSOR STREQUAL "T113")
  target_link_libraries(DuetScreen.lib PRIVATE gpiod wpa_client)
endif()

if (ENABLE_COVERAGE)
  target_compile_options(DuetScreen.lib PUBLIC --coverage)
  target_link_options(DuetScreen.lib PUBLIC --coverage)

  find_program(FIND_CMD find REQUIRED)

  add_custom_target(coverage_clean
    COMMAND ${CMAKE_COMMAND} -E echo "Removing existing .gcda files"
    COMMAND ${FIND_CMD} ${CMAKE_BINARY_DIR} -name "*.gcda" -type f -delete
    COMMAND ${FIND_CMD} ${CMAKE_BINARY_DIR} -wholename "*/src/main.cpp.gcno" -type f -delete
    COMMENT "Cleaning coverage data (*.gcda) from ${CMAKE_BINARY_DIR}"
    VERBATIM
  )

  add_dependencies(DuetScreen.lib coverage_clean)
endif()

# Link the executable with the DuetScreen core library.
# There is some static initialisation in some translation units so force linker to include everything.
target_link_libraries(DuetScreen
    -Wl,--whole-archive DuetScreen.lib -Wl,--no-whole-archive
)
# target_link_libraries(DuetScreen PRIVATE DuetScreen.lib)

# Custom target to run the executable
add_custom_target(
  binary_size ALL
  DEPENDS DuetScreen
  COMMAND size -A ${PROJECT_BINARY_DIR}/DuetScreen
  COMMENT "Running size on DuetScreen")

# Conditionally include and link SDL2_image if LV_USE_DRAW_SDL is enabled
if(CONFIG_LV_USE_DRAW_SDL)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
  find_package(SDL2_image REQUIRED)
  target_include_directories(lvgl PUBLIC ${SDL2_IMAGE_INCLUDE_DIRS})
  target_link_libraries(DuetScreen.lib PRIVATE ${SDL2_IMAGE_LIBRARIES})
endif()

# Conditionally include and link libpng if LV_USE_LIBPNG is enabled
if(CONFIG_LV_USE_LIBPNG)
  find_package(PNG REQUIRED)
  target_include_directories(lvgl PUBLIC ${PNG_INCLUDE_DIRS})
  target_link_libraries(DuetScreen.lib PRIVATE ${PNG_LIBRARIES})
endif()

# Conditionally include and link libjpeg-turbo if LV_USE_LIBJPEG_TURBO is
# enabled
if(CONFIG_LV_USE_LIBJPEG_TURBO)
  find_package(JPEG REQUIRED)
  target_include_directories(lvgl PUBLIC ${JPEG_INCLUDE_DIRS})
  target_link_libraries(DuetScreen.lib PRIVATE ${JPEG_LIBRARIES})
endif()

# Conditionally include and link FFmpeg libraries if LV_USE_FFMPEG is enabled
if(CONFIG_LV_USE_FFMPEG)
  target_link_libraries(DuetScreen.lib PRIVATE avformat avcodec avutil swscale z)
endif()

# Conditionally include and link FreeType if LV_USE_FREETYPE is enabled
if(CONFIG_LV_USE_FREETYPE)
  find_package(Freetype REQUIRED)
  target_include_directories(lvgl PUBLIC ${FREETYPE_INCLUDE_DIRS})
  target_link_libraries(DuetScreen.lib PRIVATE ${FREETYPE_LIBRARIES})
endif()

# Apply additional compile options if the build type is Debug
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Debug mode enabled")

  if(ASAN)
    message(STATUS "AddressSanitizer enabled")

    # Add AddressSanitizer flags
    set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS
        "${CMAKE_C_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
    set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")
  else()
    message(STATUS "AddressSanitizer disabled")
  endif()
endif()

# ----------------------------------------------------------------------------
# Tests
# ----------------------------------------------------------------------------
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
