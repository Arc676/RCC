set(VEHICLE_SRCS
    vehicle.cpp
    Modules/camera.cpp
    Modules/ping.cpp
    Modules/rc.cpp
    Util/config.cpp
    Util/logging.cpp)
add_executable(Vehicle ${VEHICLE_SRCS})
target_include_directories(Vehicle PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${SRC_LIBRC}
    ${LIBCAMERA_INCLUDE_DIRS})
message(${SRC_LIBRC})
target_link_libraries(Vehicle PUBLIC RC)
