target_sources(
  DuetScreen.lib PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/SideBar.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/EStop.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/EStopPresenter.cpp
                     ${CMAKE_CURRENT_SOURCE_DIR}/SideBarPresenter.cpp)
