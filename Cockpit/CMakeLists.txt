set(COCKPIT_SRCS
    dashboard.cpp
    cockpit.cpp)
add_executable(Cockpit ${COCKPIT_SRCS})
target_link_libraries(Cockpit PUBLIC RC)

target_include_directories(Cockpit PUBLIC
    ${imgui_SRC}
    ${imgui_SRC}/backends)
target_link_libraries(Cockpit PUBLIC imgui)