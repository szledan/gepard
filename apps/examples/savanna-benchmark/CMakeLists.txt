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

find_package(X11)

target_link_libraries(savanna-benchmark gepard ${X11_LIBRARIES} ${GEPARD_DEP_LIBS})
