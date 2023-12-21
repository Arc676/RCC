set(VEHICLE_SRCS
    vehicle.cpp
    Modules/ping.cpp
    Util/config.cpp
    Util/logging.cpp)
add_executable(Vehicle ${VEHICLE_SRCS})
target_include_directories(Vehicle PUBLIC ${SRC_LIBRC})
message(${SRC_LIBRC})
target_link_libraries(Vehicle PUBLIC RC)
