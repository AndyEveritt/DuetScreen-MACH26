FetchContent_Declare(
  hv
  GIT_REPOSITORY https://github.com/ithewei/libhv.git
  GIT_TAG master
)
FetchContent_MakeAvailable(hv)
set_target_properties(hv PROPERTIES 
  BUILD_SHARED OFF 
  BUILD_STATIC ON)