set(SOURCES
    main.cpp
)

add_executable(render-thread EXCLUDE_FROM_ALL ${SOURCES})
add_dependencies(examples render-thread)

target_link_libraries(render-thread gepard X11 ${LIB_DEPENDENCIES})
