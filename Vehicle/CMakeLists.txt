set(VEHICLE_SRCS
    vehicle.c
    Util/config.c
    Util/logging.c)
add_executable(Vehicle ${VEHICLE_SRCS})
target_include_directories(Vehicle PUBLIC Util)
target_link_libraries(Vehicle PUBLIC RC)