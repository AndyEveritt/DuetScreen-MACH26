# Order defines the order in the dropdown menu
target_sources(
  DuetScreen
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/DefaultTheme.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/DarkTheme.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/LightTheme.cpp
)
