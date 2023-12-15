set(COCKPIT_SRCS cockpit.cpp)
add_executable(Cockpit ${COCKPIT_SRCS})
target_link_libraries(Cockpit PUBLIC RC)