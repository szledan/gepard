set(SOURCES
    path-clock.cpp
)

add_executable(path-clock EXCLUDE_FROM_ALL ${SOURCES})
add_dependencies(examples path-clock)

target_link_libraries(path-clock gepard X11 ${LIB_DEPENDENCIES})
