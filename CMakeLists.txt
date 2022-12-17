cmake_minimum_required(VERSION 3.23)
project(proj5)
add_compile_options(-O3)
set(CMAKE_CXX_STANDARD 11)
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message("OpenMP found.")
    #   target_link_libraries(proj5 OpenMP::OpenMP_CPP)
endif()
include_directories(.)
add_executable(proj5
        Matrix.h
        test.cpp)
