FetchContent_Declare(
  nameof
  GIT_REPOSITORY https://github.com/Neargye/nameof/
  GIT_TAG v0.10.4
)
FetchContent_MakeAvailable(nameof)
target_include_directories(DuetScreen.lib PUBLIC ${nameof_SOURCE_DIR}/include)