target_sources(
  DuetScreen.lib
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/AxisJogList.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/GenericAxisControl.cpp
          ${CMAKE_CURRENT_SOURCE_DIR}/XYControl.cpp)
