add_custom_target(examples)

include_directories(${PROJECT_BINARY_DIR}/include/)

add_subdirectory(fill-path)
add_subdirectory(fill-rect)
add_subdirectory(path-clock)
add_subdirectory(savanna-benchmark)
add_subdirectory(rasterize-svg)
