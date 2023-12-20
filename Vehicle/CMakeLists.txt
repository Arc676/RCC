set(VEHICLE_SRCS
    vehicle.c
    Modules/ping.c
    Util/config.c
    Util/logging.c)
add_executable(Vehicle ${VEHICLE_SRCS})
target_include_directories(Vehicle PUBLIC ${SRC_LIBRC})
message(${SRC_LIBRC})
target_link_libraries(Vehicle PUBLIC RC)
