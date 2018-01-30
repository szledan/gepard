include(ExternalProject)

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

ExternalProject_Add(gtest
                    PREFIX ${PROJECT_SOURCE_DIR}/thirdparty/googletest
                    GIT_REPOSITORY https://github.com/google/googletest.git
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/thirdparty -DCMAKE_INSTALL_LIBDIR=lib -DCMAKE_INSTALL_INCLUDEDIR=include -DBUILD_GMOCK=OFF -DBUILD_GTEST=ON
                    BINARY_DIR ${PROJECT_BINARY_DIR}/thirdparty/googletest
                    BUILD_COMMAND make gtest
                    INSTALL_COMMAND make install > /dev/null)

set(CMAKE_THREAD_PREFER_PTHREAD)
set(THREADS_PREFER_PTHREAD_FLAG)
find_package(Threads)

add_executable(unittest ${SOURCES})
add_dependencies(unittest gtest)

target_include_directories(unittest PUBLIC ${PROJECT_BINARY_DIR}/thirdparty/include)
target_include_directories(unittest PUBLIC ${COMMON_INCLUDE_DIRS})

target_link_libraries(unittest ${PROJECT_BINARY_DIR}/thirdparty/lib/libgtest.a Threads::Threads)
