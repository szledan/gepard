cmake_minimum_required(VERSION 2.8.10)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
project(gepard C CXX)
set(PROJECT_VERSION 1)

set(EXTRA_FLAGS "-Wall -pedantic -std=c++11")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_FLAGS}")

add_subdirectory(src)
add_subdirectory(examples)
