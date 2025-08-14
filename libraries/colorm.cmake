FetchContent_Declare(
  colorm
  GIT_REPOSITORY https://github.com/soreja/colorm.git
  GIT_TAG master
)
FetchContent_MakeAvailable(colorm)
target_include_directories(DuetScreen PRIVATE ${colorm_SOURCE_DIR})