add_library(DebugDriver debug.cpp)
target_include_directories(DebugDriver PUBLIC
    ${SRC_LIBRC}
    ${CMAKE_SOURCE_DIR}/Vehicle)
