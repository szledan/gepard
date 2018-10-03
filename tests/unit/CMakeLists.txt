set(SOURCES
    gepard-unit-main.cpp
    ${PROJECT_SOURCE_DIR}/src/engines/gepard-path.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/gepard-bounding-box.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/gepard-float-point.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/gepard-transform.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/gepard-vec4.cpp
)

set(COMMON_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/src/utils
    ${PROJECT_SOURCE_DIR}/src/engines
)

add_executable(unittest ${SOURCES})

# Pthread required by gtest
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)

target_include_directories(unittest PUBLIC ${PROJECT_SOURCE_DIR}/thirdparty/include)
target_include_directories(unittest PUBLIC ${COMMON_INCLUDE_DIRS})

# TODO(dbatyai): use find_package for gtest
target_link_libraries(unittest ${PROJECT_SOURCE_DIR}/thirdparty/lib/libgtest.a ${CMAKE_THREAD_LIBS_INIT})
