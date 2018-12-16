set(IMAGE_SOURCES
    image.cpp
)

set(CHESS_SOURCES
    chess.cpp
)

add_executable(image ${IMAGE_SOURCES})
add_dependencies(examples image)

add_executable(chess ${CHESS_SOURCES})
add_dependencies(examples chess)

find_package(PNG)
find_package(X11)

target_link_libraries(image gepard ${X11_LIBRARIES} ${PNG_LIBRARIES} ${GEPARD_DEP_LIBS})
target_link_libraries(chess gepard ${X11_LIBRARIES} ${PNG_LIBRARIES} ${GEPARD_DEP_LIBS})
