# Find or bootstrap clang-format-19
set(CLANG_FORMAT_VERSION 19)
find_program(CLANG_FORMAT_19
  NAMES clang-format-19 clang-format
  DOC "Path to clang-format ${CLANG_FORMAT_VERSION}"
)


if(NOT CLANG_FORMAT_19)
  message(STATUS "clang-format-${CLANG_FORMAT_VERSION} not found. Bootstrapping a local one via Python venv.")
  find_package(Python3 COMPONENTS Interpreter REQUIRED)

  set(CLANG_FORMAT_VENV ${CMAKE_BINARY_DIR}/.clang-format-venv)
  if(UNIX)
    set(CLANG_FORMAT_19 ${CLANG_FORMAT_VENV}/bin/clang-format)
    set(_pip ${CLANG_FORMAT_VENV}/bin/pip)
  else()
    set(CLANG_FORMAT_19 ${CLANG_FORMAT_VENV}/Scripts/clang-format.exe)
    set(_pip ${CLANG_FORMAT_VENV}/Scripts/pip.exe)
  endif()

  ExternalProject_Add(clang_format_bootstrap
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ${Python3_EXECUTABLE} -m venv ${CLANG_FORMAT_VENV}
    BUILD_COMMAND ${_pip} install --upgrade pip &&
                  ${_pip} install "clang-format==${CLANG_FORMAT_VERSION}.*"
    INSTALL_COMMAND ""
    USES_TERMINAL_BUILD TRUE
  )
endif()

# Add clang-format custom target (depends on bootstrap if used)
add_custom_target(
  clang-format
  COMMAND find src/ -iname '*.h' -o -iname '*.cpp' -print | xargs ${CLANG_FORMAT_19} -i
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Running clang-format-${CLANG_FORMAT_VERSION}"
)
if(TARGET clang_format_bootstrap)
  add_dependencies(clang-format clang_format_bootstrap)
endif()