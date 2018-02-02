set(SOURCES
    gepard-unit-main.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/gepard-defs.cpp
    ${PROJECT_SOURCE_DIR}/src/utils/gepard-types.cpp
    ${PROJECT_SOURCE_DIR}/src/engines/gepard-path.cpp
)

set(COMMON_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/src/utils
    ${PROJECT_SOURCE_DIR}/src/engines
)

add_executable(unittest ${SOURCES})
add_dependencies(unittest gtest)

# Pthread required by gtest
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads)

target_include_directories(unittest PUBLIC ${PROJECT_BINARY_DIR}/thirdparty/include)
target_include_directories(unittest PUBLIC ${COMMON_INCLUDE_DIRS})

target_link_libraries(unittest ${PROJECT_BINARY_DIR}/thirdparty/lib/libgtest.a ${CMAKE_THREAD_LIBS_INIT})
