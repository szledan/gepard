add_custom_target(examples)

function(build_example directory)
  file(GLOB SOURCES ${directory}/*.cpp)
  add_executable("${directory}" EXCLUDE_FROM_ALL ${SOURCES})
  add_dependencies(examples "${directory}")
  target_link_libraries("${directory}" gepard X11 -pthread ${LIB_DEPENDENCIES})
endfunction(build_example)

include_directories(${CMAKE_BINARY_DIR}/include/)

build_example("fill-rect" "A simple example for drawing a filled rectangle with Gepard and different surfaces.")
build_example("savanna-benchmark" "A simple benchmark for testing Gepard.")
build_example("render-thread" "Simulate a rendering thread which is scheduling the command queue flush.")
