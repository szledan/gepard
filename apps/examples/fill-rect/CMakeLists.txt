set(SOURCES
    fill-rect.cpp
)

add_executable(fill-rect ${SOURCES})
add_dependencies(examples fill-rect)

target_link_libraries(fill-rect gepard X11 png ${LIB_DEPENDENCIES})
