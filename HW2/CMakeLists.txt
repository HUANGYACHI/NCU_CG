cmake_minimum_required(VERSION 3.10)
project(HW2)

set(CMAKE_CXX_STANDARD 17)

add_executable(HW2 hw2.cpp)

target_link_libraries(HW2 glut GLU GL)
