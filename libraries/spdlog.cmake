# add_subdirectory(${LIBRARIES_DIR}/spdlog)

add_compile_definitions(SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE)

find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
