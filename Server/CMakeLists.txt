cmake_minimum_required(VERSION 3.14)

project(Server)
set(EXEC_NAME Server)

include_directories(${CMAKE_SOURCE_DIR}/Server/include)
include_directories(${CMAKE_SOURCE_DIR}/TestSystem/include) 

set(SOURCE_LIB lib/Server.cpp)

add_executable(${EXEC_NAME} ${SOURCE_LIB})
target_link_libraries(${EXEC_NAME} TestSystem)

