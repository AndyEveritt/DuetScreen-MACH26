target_sources(
  DuetScreen.lib
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Commands.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/Communication.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/FileInfo.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/JsonDecoder.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/Thumbnail.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/Usb.cpp)
