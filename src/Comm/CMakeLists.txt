target_sources(
  DuetScreen
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Commands.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/Communication.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/JsonDecoder.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/Usb.cpp)
