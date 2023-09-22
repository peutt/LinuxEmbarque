cmake_minimum_required(VERSION 3.5)

project(LinuxEmbarque LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(SDL2 REQUIRED)

# Trouver la bibliothèque CURL
find_package(CURL REQUIRED)

# Trouver la bibliothèque jsoncpp (if it's not found, provide the path manually)
find_package(jsoncpp REQUIRED)

add_executable(LinuxEmbarque main.cpp)

# Include the 'jsoncpp' include directory
target_include_directories(LinuxEmbarque PRIVATE ${JSONCPP_INCLUDE_DIRS})

# Lier la bibliothèque CURL
target_link_libraries(LinuxEmbarque CURL::libcurl)
target_link_libraries(LinuxEmbarque SDL2::SDL2)
target_link_libraries(LinuxEmbarque jsoncpp_lib)
