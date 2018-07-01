set(SOURCES
    image.cpp
)

add_executable(image ${SOURCES})
add_dependencies(examples image)

find_package(PNG)
find_package(X11)

message("deps " ${GEPARD_DEP_LIBS})
target_link_libraries(image gepard ${X11_LIBRARIES} ${PNG_LIBRARIES} ${GEPARD_DEP_LIBS})
