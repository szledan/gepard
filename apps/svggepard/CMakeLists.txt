set(SOURCES
    svggepard.cpp
)

add_executable(svggepard ${SOURCES})

find_package(X11)
find_package(PNG)

target_include_directories(svggepard PRIVATE ${INCLUDE_OUTPUT_DIR})
target_link_libraries(svggepard gepard ${PNG_LIBRARIES} ${X11_LIBRARIES} ${GEPARD_DEP_LIBS})
