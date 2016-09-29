set(SOURCES
    fill-rect.cpp
)

add_executable(fill-rect EXCLUDE_FROM_ALL ${SOURCES})
add_dependencies(examples fill-rect)

target_link_libraries(fill-rect gepard X11 ${LIB_DEPENDENCIES})

