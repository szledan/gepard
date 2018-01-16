set(SOURCES
    pygepard.cpp
)

find_package(PythonLibs)
if (PYTHONLIBS_FOUND)
  include_directories(${PYTHON_INCLUDE_DIRS} ${INCLUDE_OUTPUT_DIR})

  add_library(pygepard SHARED EXCLUDE_FROM_ALL ${SOURCES})

  if(PYTHONLIBS_VERSION_STRING VERSION_LESS "3.0")
    target_compile_definitions(pygepard PUBLIC PYTHON2)
  else()
    target_compile_definitions(pygepard PUBLIC PYTHON3)
  endif()

  SET_TARGET_PROPERTIES(pygepard PROPERTIES PREFIX "")
  add_dependencies(examples pygepard)

  target_link_libraries(pygepard gepard ${PYTHON_LIBRARIES} X11 ${LIB_DEPENDENCIES})
endif()
