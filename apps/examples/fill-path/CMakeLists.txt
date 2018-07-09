set(SOURCES
    fill-path.cpp
)

add_executable(fill-path ${SOURCES})
add_dependencies(examples fill-path)

find_package(X11)
find_package(PNG)

target_link_libraries(fill-path gepard ${X11_LIBRARIES} ${PNG_LIBRARIES} ${GEPARD_DEP_LIBS})
