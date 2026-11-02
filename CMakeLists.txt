
cmake_minimum_required(VERSION 3.20)

project(PetitCompagnon)

set(CMAKE_CXX_STANDARD 17)

add_executable(Bonhomme WIN32 main.cpp)

target_link_libraries(Bonhomme PRIVATE
    user32
    gdi32
)
