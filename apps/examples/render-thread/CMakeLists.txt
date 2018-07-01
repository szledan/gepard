set(SOURCES
    main.cpp
)

add_executable(render-thread ${SOURCES})
add_dependencies(examples render-thread)

find_package(X11)

target_link_libraries(render-thread gepard ${X11_LIBRARIES} ${GEPARD_DEP_LIBS})
