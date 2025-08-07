set(BUILD_SHARED OFF CACHE BOOL "Build shared library")
set(BUILD_STATIC ON CACHE BOOL "Build static library")

FetchContent_Declare(
  hv
  GIT_REPOSITORY https://github.com/ithewei/libhv.git
  GIT_TAG master
)
FetchContent_MakeAvailable(hv)