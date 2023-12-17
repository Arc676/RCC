set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

set(LIB_SRCS camera.c)
add_library(RC ${LIB_SRCS})
target_link_libraries(RC PUBLIC Threads::Threads)