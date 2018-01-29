set(SOURCES
    pygepard.cpp
)

find_package(PythonLibs)
add_library(pygepard SHARED ${SOURCES})

target_include_directories(pygepard PUBLIC ${PYTHON_INCLUDE_DIRS} ${INCLUDE_OUTPUT_DIR})
SET_TARGET_PROPERTIES(pygepard PROPERTIES PREFIX "")

target_link_libraries(pygepard gepard ${PYTHON_LIBRARIES} X11 ${LIB_DEPENDENCIES})
