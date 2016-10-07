add_custom_target(examples)

include_directories(${CMAKE_BINARY_DIR}/include/)

add_subdirectory(fill-rect EXCLUDE_FROM_ALL)
add_subdirectory(savanna-benchmark EXCLUDE_FROM_ALL)
