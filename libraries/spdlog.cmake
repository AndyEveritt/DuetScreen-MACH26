FetchContent_Declare(
  fmt
  SYSTEM # Mark as system to suppress warnings from this external library
  GIT_REPOSITORY https://github.com/fmtlib/fmt
  GIT_TAG 12.1.0
)
FetchContent_MakeAvailable(fmt)

target_compile_definitions(fmt PUBLIC FMT_USE_EXCEPTIONS=0)

# Force spdlog to use the external fmt provided above
# Must be set before FetchContent_MakeAvailable(spdlog)
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "Use external fmt library" FORCE)

FetchContent_Declare(
  spdlog
  SYSTEM # Mark as system to suppress warnings from this external library
  GIT_REPOSITORY https://github.com/gabime/spdlog
  GIT_TAG v1.16.0
)
FetchContent_MakeAvailable(spdlog)

target_compile_definitions(
    spdlog PRIVATE
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE
)
