set(BUILD_SHARED OFF CACHE BOOL "Build shared library")
set(BUILD_STATIC ON CACHE BOOL "Build static library")
set(WITH_OPENSSL ON CACHE BOOL "For HTTPS support" FORCE)

FetchContent_Declare(
  hv
  SYSTEM # Mark as system to suppress warnings from this external library
  GIT_REPOSITORY https://github.com/ithewei/libhv.git
  GIT_TAG v1.3.4
)
FetchContent_MakeAvailable(hv)
# target_include_directories(DuetScreen.lib PUBLIC ${hv_SOURCE_DIR})