set(COCKPIT_SRCS
    Modules/camera.cpp
    Modules/ping.cpp
    Modules/rc.cpp
    Modules/rc_default_controls.cpp
    dashboard.cpp
    cockpit.cpp)
add_executable(Cockpit ${COCKPIT_SRCS})
target_include_directories(Cockpit PUBLIC
    ${SRC_LIBRC}
    ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Cockpit PUBLIC RC)

target_include_directories(Cockpit PUBLIC
    ${imgui_SRC}
    ${imgui_SRC}/backends)
target_link_libraries(Cockpit PUBLIC imgui)
