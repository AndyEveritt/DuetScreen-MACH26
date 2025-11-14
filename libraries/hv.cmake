set(BUILD_SHARED OFF CACHE BOOL "Build shared library")
set(BUILD_STATIC ON CACHE BOOL "Build static library")

FetchContent_Declare(
  hv
  GIT_REPOSITORY https://github.com/ithewei/libhv.git
  GIT_TAG v1.3.4
)
FetchContent_MakeAvailable(hv)
# target_include_directories(DuetScreen.lib PUBLIC ${hv_SOURCE_DIR})