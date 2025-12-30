option(TRACY_ENABLE ON)
option(TRACY_LTO ON)
option(TRACY_NO_SAMPLING ON)
option(TRACY_ON_DEMAND "Only enable Tracy when a server is connected" ON)
add_subdirectory(${LIBRARIES_DIR}/tracy)

target_compile_definitions(TracyClient PUBLIC
    TRACY_ENABLE=${TRACY_ENABLE}
    # TRACY_NO_SAMPLING={TRACY_NO_SAMPLING}
)