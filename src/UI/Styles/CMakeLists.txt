target_sources(
  DuetScreen.lib
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Color.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Font.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Styles.cpp
)

add_subdirectory(Themes)