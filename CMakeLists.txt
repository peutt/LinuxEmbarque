cmake_minimum_required(VERSION 3.5)

project(LinuxEmbarque LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(SDL2 REQUIRED)

# Trouver la bibliothèque CURL
find_package(CURL REQUIRED)

add_executable(LinuxEmbarque main.cpp)

# Lier la bibliothèque CURL
target_link_libraries(LinuxEmbarque CURL::libcurl)
target_link_libraries(LinuxEmbarque SDL2::SDL2)

