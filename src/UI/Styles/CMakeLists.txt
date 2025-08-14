target_sources(
  DuetScreen
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Color.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Styles.cpp
)

add_subdirectory(Themes)