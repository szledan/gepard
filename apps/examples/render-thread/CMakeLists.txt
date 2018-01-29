set(SOURCES
    main.cpp
)

add_executable(render-thread ${SOURCES})
add_dependencies(examples render-thread)

target_link_libraries(render-thread gepard X11 ${GEPARD_DEP_LIBS})
