set(BUILD_SHARED OFF CACHE BOOL "Build shared library")
set(BUILD_STATIC ON CACHE BOOL "Build static library")

FetchContent_Declare(
  hv
  GIT_REPOSITORY https://github.com/ithewei/libhv.git
  GIT_TAG e1015fbda48ed2d2c33d84977bc3134088a7feef
)
FetchContent_MakeAvailable(hv)
# target_include_directories(DuetScreen.lib PUBLIC ${hv_SOURCE_DIR})