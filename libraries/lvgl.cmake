option(LV_CONF_BUILD_DISABLE_DEMOS "Disable building of demos" ON)
option(LV_CONF_BUILD_DISABLE_EXAMPLES "Disable building of examples" ON)
option(LV_BUILD_SET_CONFIG_OPTS "Convert LVGL configuration options to CMake cache" ON)

add_compile_definitions(LV_USE_OS=LV_OS_NONE)

set(LV_LVGL_H_INCLUDE_SIMPLE
    OFF
    CACHE BOOL INTERNAL FORCE)
set(LV_CONF_INCLUDE_SIMPLE
    ON
    CACHE BOOL INTERNAL FORCE)
set(LV_CONF_PATH ${PROJECT_SOURCE_DIR}/lv_conf.h
    CACHE STRING "Path to lv_conf.h")

# Add LVGL subdirectory
# set(BUILD_SHARED_LIBS OFF CACHE BOOL FORCE "Build shared libraries")
add_subdirectory(${LIBRARIES_DIR}/lvgl)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR}
                                       ${SDL2_INCLUDE_DIRS} ${LIBRARIES_DIR})

# include(${CMAKE_BINARY_DIR}/libraries/lvgl/lv_conf.cmake)

# Drivers add_subdirectory(${LIBRARIES_DIR}/lv_drivers)
# target_compile_definitions( lv_drivers PUBLIC
# $<$<BOOL:${LV_LVGL_H_INCLUDE_SIMPLE}>:LV_LVGL_H_INCLUDE_SIMPLE>
# $<$<BOOL:${LV_CONF_INCLUDE_SIMPLE}>:LV_CONF_INCLUDE_SIMPLE> USE_SUNXIFB_G2D=1)

# target_include_directories(lv_drivers PUBLIC ${PROJECT_SOURCE_DIR}
# ${LIBRARIES_DIR})
