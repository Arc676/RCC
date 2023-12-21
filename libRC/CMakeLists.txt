set(SRC_LIBRC ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")

set(LIB_SRCS
    Stream/netstream.cpp
    Peripherals/camera.cpp)
add_library(RC ${LIB_SRCS})
target_include_directories(RC PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
