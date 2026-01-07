cmake_minimum_required(VERSION 3.13)

project(lvgl_tracy LANGUAGES CXX)

# Build a small static library that provides LVGL tracing helpers
add_library(lvgl_tracy STATIC
	tracing.cpp
)

target_compile_features(lvgl_tracy PUBLIC cxx_std_23)

target_compile_options(lvgl_tracy PRIVATE
	-Wall
	-Wextra
	-Werror
	-Wpedantic
	-Wno-unused-parameter
)

if(PROCESSOR STREQUAL "T113" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_options(lvgl_tracy PRIVATE -Wno-error=cpp) # required because of fortify_source warnings
endif()

# Public headers (for include in consumers)
target_include_directories(lvgl_tracy
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		$<INSTALL_INTERFACE:include>
)

# Link against Tracy client; this also propagates its public include dirs
target_link_libraries(lvgl_tracy
	PUBLIC
		Tracy::TracyClient
)

# Provide an alias for consistency with other libs
add_library(lvgl_tracy::lvgl_tracy ALIAS lvgl_tracy)

