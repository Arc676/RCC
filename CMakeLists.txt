cmake_minimum_required(VERSION 3.28)

project(
    RCC
    VERSION 1.0.0
    HOMEPAGE_URL https://github.com/Arc676/RCC
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_subdirectory(dependencies)

add_subdirectory(libRC)
add_subdirectory(Cockpit)
add_subdirectory(Vehicle)