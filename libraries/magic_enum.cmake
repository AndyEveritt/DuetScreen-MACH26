FetchContent_Declare(
  magic_enum
  SYSTEM # Mark as system to suppress warnings from this external library
  GIT_REPOSITORY https://github.com/Neargye/magic_enum/
  GIT_TAG v0.9.7
)
FetchContent_MakeAvailable(magic_enum)