set(VEHICLE_SRCS vehicle.c)
add_executable(Vehicle ${VEHICLE_SRCS})
target_link_libraries(Vehicle PUBLIC RC)