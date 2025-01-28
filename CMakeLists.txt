cmake_minimum_required(VERSION 3.10)
project(DuetScreen C CXX)

# Set C and C++ standards
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(LIBRARIES_DIR ${PROJECT_SOURCE_DIR}/libraries)
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# Remove ARM-specific compile and linker options
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

set(MAIN_SOURCES "")

# ----------------------------------------------------------------------------
# Libraries
# ----------------------------------------------------------------------------

# FreeRTOS
include(${LIBRARIES_DIR}/FreeRTOS.cmake)

# LVGL
include(${LIBRARIES_DIR}/lvgl.cmake)

# Networking
add_subdirectory(${LIBRARIES_DIR}/libhv)

# Find libusb package
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED libusb-1.0)

# Find and include SDL2 library
find_package(SDL2 REQUIRED)

# Find json library
set(JSON_BuildTests
    OFF
    CACHE INTERNAL "")
add_subdirectory(${LIBRARIES_DIR}/json)

# ----------------------------------------------------------------------------
# Set source files for executable
# ----------------------------------------------------------------------------
if(USE_FREERTOS)
  list(
    APPEND
    MAIN_SOURCES
    ${SRC_DIR}/freertos_main.cpp
    ${SRC_DIR}/FreeRTOS_Posix_Port.c
    ${FREERTOS_SOURCES} # Add only if USE_FREERTOS is enabled
  )
endif()

file(GLOB_RECURSE DUET_LIB_SOURCES ${LIBRARIES_DIR}/Duet3D/**.c
     ${LIBRARIES_DIR}/Duet3D/**.cpp)
# file(GLOB_RECURSE COMM_SOURCES ${SRC_DIR}/Comm/**.c ${SRC_DIR}/Comm/**.cpp)
file(GLOB_RECURSE HW_SOURCES ${SRC_DIR}/Hardware/**.c
     ${SRC_DIR}/Hardware/**.cpp)
list(REMOVE_ITEM HW_SOURCES ${SRC_DIR}/Hardware/Usb.cpp)

file(GLOB_RECURSE OM_SOURCES ${SRC_DIR}/ObjectModel/**.c
     ${SRC_DIR}/ObjectModel/**.cpp)
# list(REMOVE_ITEM OM_SOURCES ${SRC_DIR}/ObjectModel/Job.cpp)

file(GLOB_RECURSE SUBSCRIBER_SOURCES ${SRC_DIR}/Subscribers/**.c
     ${SRC_DIR}/Subscribers/**.cpp)
file(GLOB_RECURSE UI_SOURCES ${SRC_DIR}/UI/**.c ${SRC_DIR}/UI/**.cpp)

file(GLOB_RECURSE UTILS_SOURCES ${SRC_DIR}/utils/**.c ${SRC_DIR}/utils/**.cpp)

list(
  APPEND
  MAIN_SOURCES
  ${SRC_DIR}/main.cpp
  ${SRC_DIR}/lv_i18n/lv_i18n.c
  ${SRC_DIR}/mouse_cursor_icon.c
  ${SRC_DIR}/Debug.cpp
  ${SRC_DIR}/DebugCommands.cpp
  ${SRC_DIR}/Comm/Commands.cpp
  ${SRC_DIR}/Comm/Communication.cpp
  ${SRC_DIR}/Comm/JsonDecoder.cpp
  ${SRC_DIR}/Comm/Usb.cpp
  ${DUET_LIB_SOURCES}
  ${COMM_SOURCES}
  ${HW_SOURCES}
  ${OM_SOURCES}
  ${SUBSCRIBER_SOURCES}
  ${UI_SOURCES}
  ${UTILS_SOURCES})

add_executable(DuetScreen ${MAIN_SOURCES})
target_include_directories(DuetScreen PUBLIC ${SRC_DIR})

# Include libusb headers
target_include_directories(DuetScreen PRIVATE ${LIBUSB_INCLUDE_DIRS})

# ----------------------------------------------------------------------------
# Add Definitions
# ----------------------------------------------------------------------------
target_compile_definitions(DuetScreen PRIVATE DEBUG)
target_compile_definitions(DuetScreen PRIVATE MULTITHREADED)

if(PROCESSOR STREQUAL "T113")
  target_compile_definitions(lvgl PUBLIC LV_USE_LINUX_FBDEV=1)
  target_compile_definitions(lvgl PUBLIC LV_USE_EVDEV=1)
  target_compile_definitions(DuetScreen PUBLIC T113=1)
  # add_compile_definitions(T113=1)
elseif(PROCESSOR STREQUAL "Simulation")
  target_compile_definitions(lvgl PUBLIC LV_USE_SDL=1)
  target_compile_definitions(DuetScreen PRIVATE SIMULATION=1)
endif()

if(DEBUG_BORDERS)
  target_compile_definitions(DuetScreen PRIVATE DEBUG_BORDERS=1)
endif()

if(LOG_TIMESTAMPS)
  target_compile_definitions(DuetScreen PRIVATE LOG_TIMESTAMPS=1)
endif()

if(ASYNC_RR_MODEL)
  target_compile_definitions(DuetScreen PRIVATE ASYNC_RR_MODEL=1)
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

define_file_relpath_for_sources(DuetScreen)

# Define LVGL configuration as a simple include
target_compile_definitions(DuetScreen PRIVATE LV_CONF_INCLUDE_SIMPLE)
target_link_libraries(
  DuetScreen
  PRIVATE lvgl
          # lvgl::examples lvgl::demos
          lvgl::thorvg
          ${SDL2_LIBRARIES}
          ${LIBUSB_LIBRARIES} # Link libusb library
          m
          pthread
          hv
          nlohmann_json::nlohmann_json)

# Only link freertos_config if the FreeRTOS directory exists
if(USE_FREERTOS)
  target_link_libraries(DuetScreen freertos_config freertos_kernel)
endif()

# Custom target to run the executable
add_custom_target(
  binary_size ALL
  DEPENDS DuetScreen
  COMMAND size -A ${PROJECT_BINARY_DIR}/DuetScreen
  COMMENT "Running size on DuetScreen")

# Conditionally include and link SDL2_image if LV_USE_DRAW_SDL is enabled
if(LV_USE_DRAW_SDL)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
  find_package(SDL2_image REQUIRED)
  target_include_directories(lvgl PUBLIC ${SDL2_IMAGE_INCLUDE_DIRS})
  target_link_libraries(DuetScreen ${SDL2_IMAGE_LIBRARIES})
endif()

# Conditionally include and link libpng if LV_USE_LIBPNG is enabled
if(LV_USE_LIBPNG)
  find_package(PNG REQUIRED)
  target_include_directories(lvgl PUBLIC ${PNG_INCLUDE_DIRS})
  target_link_libraries(DuetScreen ${PNG_LIBRARIES})
endif()

# Conditionally include and link libjpeg-turbo if LV_USE_LIBJPEG_TURBO is
# enabled
if(LV_USE_LIBJPEG_TURBO)
  find_package(JPEG REQUIRED)
  target_include_directories(lvgl PUBLIC ${JPEG_INCLUDE_DIRS})
  target_link_libraries(DuetScreen ${JPEG_LIBRARIES})
endif()

# Conditionally include and link FFmpeg libraries if LV_USE_FFMPEG is enabled
if(LV_USE_FFMPEG)
  target_link_libraries(DuetScreen avformat avcodec avutil swscale z)
endif()

# Conditionally include and link FreeType if LV_USE_FREETYPE is enabled
if(LV_USE_FREETYPE)
  find_package(Freetype REQUIRED)
  target_include_directories(lvgl PUBLIC ${FREETYPE_INCLUDE_DIRS})
  target_link_libraries(DuetScreen ${FREETYPE_LIBRARIES})
endif()

# Apply additional compile options if the build type is Debug
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Debug mode enabled")

  target_compile_options(
    lvgl
    PRIVATE -pedantic-errors
            -Wall
            -Wclobbered
            -Wdeprecated
            -Wdouble-promotion
            -Wempty-body
            -Wextra
            -Wformat-security
            -Wmaybe-uninitialized
            # -Wmissing-prototypes
            -Wpointer-arith
            -Wmultichar
            -Wno-pedantic # ignored for now, we convert functions to pointers
                          # for properties table.
            -Wreturn-type
            -Wshadow
            -Wshift-negative-value
            -Wsizeof-pointer-memaccess
            -Wtype-limits
            -Wundef
            -Wuninitialized
            -Wunreachable-code
            -Wfloat-conversion
            -Wstrict-aliasing)

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
