cmake_minimum_required(VERSION 3.0)
project(rcgc) 

SET(CMAKE_C_COMPILER g++)
if(CMAKE_COMPILER_IS_GNUCXX)
    add_compile_options(-std=c++11 -g)
    message(STATUS "optional:-std=c++11")
endif(CMAKE_COMPILER_IS_GNUCXX)

add_executable(rcgc main.cpp rcgc_shared_ptr.cpp) 
 
target_link_libraries(rcgc pthread)
