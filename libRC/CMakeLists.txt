set(SRC_LIBRC ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")

find_package(PkgConfig REQUIRED)
pkg_check_modules(libcamera REQUIRED IMPORTED_TARGET libcamera)

set(LIB_SRCS
    Stream/buffer.cpp
    Stream/camdata.cpp
    Stream/netstream.cpp
    Stream/rc.cpp
    Peripherals/camera.cpp)
add_library(RC ${LIB_SRCS})
target_include_directories(RC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${libcamera_INCLUDE_DIRS})
target_link_libraries(RC PUBLIC ${libcamera_LIBRARIES})

set(LIBCAMERA_INCLUDE_DIRS ${libcamera_INCLUDE_DIRS} CACHE INTERNAL "")
