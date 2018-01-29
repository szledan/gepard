set(SOURCES
    fill-path.cpp
)

add_executable(fill-path ${SOURCES})
add_dependencies(examples fill-path)

target_link_libraries(fill-path gepard X11 png ${LIB_DEPENDENCIES})
