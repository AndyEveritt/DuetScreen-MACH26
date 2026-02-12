# Order defines the order in the dropdown menu
target_sources(
  DuetScreen.lib
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/CustomTheme.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DefaultTheme.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DuetScreenTheme.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/FlatTheme.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/GreyScaleTheme.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/IndustrialTheme.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/SoftTheme.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/NeonTheme.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/RetroTheme.cpp
)
