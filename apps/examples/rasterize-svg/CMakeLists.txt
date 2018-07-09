set(SOURCES
    rasterize-svg.cpp
)

add_executable(rasterize-svg ${SOURCES})
add_dependencies(examples rasterize-svg)

find_package(X11)
find_package(PNG)

target_link_libraries(rasterize-svg gepard ${PNG_LIBRARIES} ${X11_LIBRARIES} ${GEPARD_DEP_LIBS})
