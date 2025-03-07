target_sources(
  DuetScreen
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/base64.c
  ${CMAKE_CURRENT_SOURCE_DIR}/bmp.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/png.cpp
)