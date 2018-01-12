set(SOURCES
    pygepard.cpp
)

find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS} ${INCLUDE_OUTPUT_DIR})

add_library(pygepard SHARED EXCLUDE_FROM_ALL ${SOURCES})
SET_TARGET_PROPERTIES(pygepard PROPERTIES PREFIX "")
add_dependencies(examples pygepard)

target_link_libraries(pygepard gepard ${PYTHON_LIBRARIES} X11 ${LIB_DEPENDENCIES})
