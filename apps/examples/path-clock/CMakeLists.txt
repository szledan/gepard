set(SOURCES
    path-clock.cpp
)

add_executable(path-clock ${SOURCES})
add_dependencies(examples path-clock)

find_package(X11)

target_link_libraries(path-clock gepard ${X11_LIBRARIES} ${GEPARD_DEP_LIBS})
