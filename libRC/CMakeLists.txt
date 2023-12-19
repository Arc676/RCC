set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

set(SRC_LIBRC ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")

set(LIB_SRCS
    Stream/netstream.c
    camera.c)
add_library(RC ${LIB_SRCS})
target_link_libraries(RC PUBLIC Threads::Threads)
