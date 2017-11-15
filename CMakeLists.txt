cmake_minimum_required(VERSION 2.8.12)
project(PyAsync CXX)

option(BLOCKING_OUTPUT "Enable mutex around std::cout in C++" ON)
if(BLOCKING_OUTPUT)
    add_definitions(-DBLOCKING_OUTPUT)
endif()

add_subdirectory(pybind11)
pybind11_add_module(async ${CMAKE_SOURCE_DIR}/async.cc)
configure_file(${CMAKE_SOURCE_DIR}/async.py
  ${CMAKE_BINARY_DIR}/async.py COPYONLY)
source_group("Python" FILES ${CMAKE_SOURCE_DIR}/async.py)
