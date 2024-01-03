# https://gist.github.com/jeffamstutz/992723dfabac4e3ffff265eb71a24cd9
FetchContent_Populate(imgui
  URL https://github.com/ocornut/imgui/archive/refs/tags/v1.90.zip
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui
)

set(imgui_SRC ${imgui_SOURCE_DIR} CACHE INTERNAL "")
find_package(SDL2 REQUIRED)

add_library(imgui STATIC
  imgui/imgui.cpp
  imgui/imgui_draw.cpp
  imgui/imgui_demo.cpp
  imgui/imgui_tables.cpp
  imgui/imgui_widgets.cpp

  imgui/backends/imgui_impl_sdl2.cpp
  imgui/backends/imgui_impl_sdlrenderer2.cpp
)

target_link_libraries(imgui PUBLIC SDL2 ${SDL2_LIBRARIES})

target_include_directories(imgui
PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/imgui
  ${CMAKE_CURRENT_LIST_DIR}/imgui/backends
  ${SDL2_INCLUDE_DIRS}
)
