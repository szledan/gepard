set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

function(build_example directory)
  file(GLOB SOURCES ${directory}/*.cpp)
  add_executable("${directory}" ${SOURCES})
  target_link_libraries("${directory}" gepard EGL GLESv2 X11)
endfunction(build_example)


include_directories(${CMAKE_BINARY_DIR}/src/)
# TODO: remove:
include_directories(${CMAKE_SOURCE_DIR}/src/)

#build_example("triangle" "")
