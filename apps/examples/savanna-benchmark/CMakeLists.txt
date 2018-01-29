set(SOURCES
    main.cpp
    savanna-benchmarks.cpp
    savanna.cpp
)

set(INCLUDE_DIRS
    .
)

include_directories(${INCLUDE_DIRS})

add_executable(savanna-benchmark ${SOURCES})
add_dependencies(examples savanna-benchmark)

target_link_libraries(savanna-benchmark gepard X11 ${GEPARD_DEP_LIBS})
