option(EXTERNAL_DRIVER "Use a vehicle driver external to RCC" OFF)
option(VEHICLE_DRIVER "Driver for Vehicle server" "N/A")

if("${VEHICLE_DRIVER}" STREQUAL "N/A")
    message(FATAL_ERROR "Cannot build vehicle without specifying a driver")
endif()

if(NOT EXTERNAL_DRIVER)
    message(STATUS "Using provided driver at Drivers/" ${VEHICLE_DRIVER})
    add_subdirectory(Drivers/${VEHICLE_DRIVER})
endif()

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
target_link_libraries(Vehicle PUBLIC RC)
target_link_libraries(Vehicle PUBLIC ${VEHICLE_DRIVER}Driver)
