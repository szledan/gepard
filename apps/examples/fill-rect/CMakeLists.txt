set(SOURCES
    fill-rect.cpp
)

add_executable(fill-rect ${SOURCES})
add_dependencies(examples fill-rect)

find_package(X11)
find_package(PNG)

target_link_libraries(fill-rect gepard ${X11_LIBRARIES} ${PNG_LIBRARIES} ${GEPARD_DEP_LIBS})
