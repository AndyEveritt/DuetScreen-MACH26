option(LV_BUILD_SET_CONFIG_OPTS "Convert LVGL configuration options to CMake cache" ON)

set(LV_LVGL_H_INCLUDE_SIMPLE
    OFF
    CACHE BOOL INTERNAL FORCE)
set(LV_CONF_INCLUDE_SIMPLE
    ON
    CACHE BOOL INTERNAL FORCE)
set(LV_CONF_PATH ${PROJECT_SOURCE_DIR}/lv_conf.h
    CACHE STRING FORCE "Path to lv_conf.h")

# Add LVGL subdirectory
add_subdirectory(${LIBRARIES_DIR}/lvgl)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR}
                                       ${SDL2_INCLUDE_DIRS}
                                       ${LIBRARIES_DIR}
                                      #  ${LIBRARIES_DIR}/tracy/public
)

target_link_libraries(lvgl PUBLIC Tracy::TracyClient)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  target_compile_definitions(lvgl PUBLIC
                              # LV_USE_ASSERT_OBJ=1 # significantly increases unit test time and decreases frame rate
                              LV_USE_LOG=1
                              LV_LOG_PRINTF=1
                              LV_USE_SYSMON=1
                              LV_USE_PERF_MONITOR=1
                              LV_USE_MEM_MONITOR=1
                              LV_USE_PROFILER=1
                              LV_PROFILER_INCLUDE="tracy/TracyC.h"
  )

  target_compile_options(
    lvgl
    PRIVATE -pedantic-errors
            -Wall
            -Wdeprecated
            -Wdouble-promotion
            -Wempty-body
            -Wextra
            -Wformat-security
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
            
  if(NOT APPLE)
    target_compile_options(
      lvgl
      PRIVATE 
              -Wclobbered
              -Wmaybe-uninitialized
              )
  endif()
endif()


if(LV_BUILD_SET_CONFIG_OPTS)
  get_target_property(LV_CONF_DEFINES lvgl COMPILE_DEFINITIONS)
  # Export CONFIG_LV_* compile definitions as global cache variables
  if(LV_CONF_DEFINES)
    foreach(_def IN LISTS LV_CONF_DEFINES)
      # Only mirror variables that start with `LV_`
      if(_def MATCHES "^LV_")
        if(_def MATCHES "^([^=]+)=(.*)$")
          set(_name "CONFIG_${CMAKE_MATCH_1}")
          set(_value "${CMAKE_MATCH_2}")
        else()
          # Definitions without an explicit value are treated as 1
          set(_name "CONFIG_${_def}")
          set(_value "1")
        endif()
        # Make it globally available via the cache
        set(${_name} "${_value}" CACHE STRING "Mirrored from lvgl COMPILE_DEFINITIONS" FORCE)
      endif()
    endforeach()
  endif()
endif()
