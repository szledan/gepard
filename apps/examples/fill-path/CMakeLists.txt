set(SOURCES
    fill-path.cpp
)

add_executable(fill-path ${SOURCES})
add_dependencies(examples fill-path)

find_package(PNG)

target_link_libraries(fill-path gepard ${PNG_LIBRARIES} ${GEPARD_DEP_LIBS})
