cmake_minimum_required(VERSION 3.14)

project(NetworkTestingSystem)

add_compile_options(-std=c++23)

add_subdirectory(TestSystem)
add_subdirectory(GUI)
add_subdirectory(Server)
add_subdirectory(Client)
