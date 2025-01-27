# Define options for LVGL with default values (OFF)
option(LV_USE_DRAW_SDL "Use SDL draw unit" OFF)
option(LV_USE_LIBPNG "Use libpng to decode PNG" OFF)
option(LV_USE_LIBJPEG_TURBO "Use libjpeg turbo to decode JPEG" OFF)
option(LV_USE_FFMPEG "Use libffmpeg to display video using lv_ffmpeg" OFF)
option(LV_USE_FREETYPE "Use freetype library" OFF)

option(LV_CONF_BUILD_DISABLE_DEMOS "Disable building of demos" ON)
option(LV_CONF_BUILD_DISABLE_EXAMPLES "Disable building of examples" ON)

# Add compile definitions based on the selected options
add_compile_definitions($<$<BOOL:${LV_USE_DRAW_SDL}>:LV_USE_DRAW_SDL=1>)
add_compile_definitions($<$<BOOL:${LV_USE_LIBPNG}>:LV_USE_LIBPNG=1>)
add_compile_definitions(
  $<$<BOOL:${LV_USE_LIBJPEG_TURBO}>:LV_USE_LIBJPEG_TURBO=1>)
add_compile_definitions($<$<BOOL:${LV_USE_FFMPEG}>:LV_USE_FFMPEG=1>)
add_compile_definitions(LV_USE_OS=LV_OS_PTHREAD)
add_compile_definitions($<$<BOOL:${USE_FREERTOS}>:LV_USE_OS=LV_OS_FREERTOS>)

set(LV_LVGL_H_INCLUDE_SIMPLE
    OFF
    CACHE BOOL INTERNAL FORCE)
set(LV_CONF_INCLUDE_SIMPLE
    ON
    CACHE STRING INTERNAL FORCE)

# Add LVGL subdirectory
add_subdirectory(${LIBRARIES_DIR}/lvgl)
target_include_directories(lvgl PUBLIC ${PROJECT_SOURCE_DIR}
                                       ${SDL2_INCLUDE_DIRS} ${LIBRARIES_DIR})

add_subdirectory(${LIBRARIES_DIR}/lv_drivers)

# Drivers
target_compile_definitions(
  lv_drivers
  PUBLIC $<$<BOOL:${LV_LVGL_H_INCLUDE_SIMPLE}>:LV_LVGL_H_INCLUDE_SIMPLE>
         $<$<BOOL:${LV_CONF_INCLUDE_SIMPLE}>:LV_CONF_INCLUDE_SIMPLE>
         USE_SUNXIFB_G2D=1
         )

target_include_directories(lv_drivers PUBLIC ${PROJECT_SOURCE_DIR} ${LIBRARIES_DIR})
