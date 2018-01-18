include(ExternalProject)

set(EXTRA_FLAGS " -Wall -pedantic -std=c++11 ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_FLAGS}")

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
                    PREFIX ${PROJECT_SOURCE_DIR}/tools/frameworks/googletest
                    GIT_REPOSITORY https://github.com/google/googletest.git
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${PROJECT_SOURCE_DIR}/tools/frameworks/googletest
                    INSTALL_COMMAND make install > /dev/null)
set_target_properties(gtest PROPERTIES EXCLUDE_FROM_ALL TRUE)

include_directories(${PROJECT_SOURCE_DIR}/tools/frameworks/googletest/include)
include_directories(${COMMON_INCLUDE_DIRS})

add_executable(unit EXCLUDE_FROM_ALL ${SOURCES})
add_dependencies(unit gtest)
target_link_libraries(unit ${PROJECT_SOURCE_DIR}/tools/frameworks/googletest/lib/libgtest.a pthread)
