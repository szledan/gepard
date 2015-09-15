set(EXTRA_FLAGS " -Wall -pedantic -std=c++11 ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_FLAGS}")

include_directories(${CMAKE_BINARY_DIR}/include/)
# TODO: remove:
include_directories(${CMAKE_SOURCE_DIR}/src/)

file(GLOB SOURCES *.cpp)
if( SOURCES )
  add_executable(unit ${SOURCES})
  target_link_libraries(unit gepard X11 ${LIB_DEPENDENCIES})
endif()
