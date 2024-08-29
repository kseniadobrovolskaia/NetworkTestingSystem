cmake_minimum_required(VERSION 3.14)

project(TestSystem)
set(LIB_NAME TestSystem)

add_compile_options(-std=c++20 -fPIC)

include_directories(${CMAKE_SOURCE_DIR}/TestSystem/include) 

set(SOURCE_LIB lib/TestSystem.cpp)

add_library(TestSystem STATIC ${SOURCE_LIB})

add_executable(testExample main.cpp)
target_link_libraries(testExample TestSystem)

