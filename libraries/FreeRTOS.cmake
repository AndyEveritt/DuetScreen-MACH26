# Set the correct FreeRTOS port for your system (e.g., Posix for WSL)
set(FREERTOS_PORT GCC_POSIX CACHE STRING "Port for FreeRTOS on Posix environment")

option(USE_FREERTOS "Enable FreeRTOS" OFF) # Turn this on to enable FreeRTOS

if(USE_FREERTOS)
    message(STATUS "FreeRTOS is enabled")

    # FreeRTOS integration when USE_FREERTOS is enabled
    add_library(freertos_config INTERFACE)
    target_include_directories(freertos_config SYSTEM INTERFACE ${PROJECT_SOURCE_DIR}/config)
    target_compile_definitions(freertos_config INTERFACE projCOVERAGE_TEST=0)

    # Add FreeRTOS as a subdirectory
    add_subdirectory(FreeRTOS)

    # FreeRTOS-specific include directories
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/include)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/portable/ThirdParty/GCC/Posix)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/config)

    # Add FreeRTOS sources
    file(GLOB FREERTOS_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/portable/MemMang/heap_4.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/FreeRTOS/portable/ThirdParty/GCC/Posix/*.c"
    )
else()
    message(STATUS "FreeRTOS is disabled")
    set(FREERTOS_SOURCES "")  # No FreeRTOS sources if FreeRTOS is disabled
endif()