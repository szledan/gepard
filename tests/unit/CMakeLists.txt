include(ExternalProject)

set(EXTRA_FLAGS " -Wall -pedantic -std=c++11 ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_FLAGS}")

set(SOURCES
    gepard-types-tests.cpp
)

set(COMMON_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/engines
    ${CMAKE_SOURCE_DIR}/src/surfaces
    ${CMAKE_SOURCE_DIR}/src/utils
)

set(GLES2_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/src/engines/gles2
)

set(VULKAN_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/src/engines/vulkan
)

set(SOFTWARE_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/src/engines/software
)

ExternalProject_Add(gtest
                    PREFIX ${CMAKE_SOURCE_DIR}/tools/frameworks/googletest
                    GIT_REPOSITORY https://github.com/google/googletest.git
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/tools/frameworks/googletest
                    INSTALL_COMMAND make install > /dev/null)
set_target_properties(gtest PROPERTIES EXCLUDE_FROM_ALL TRUE)

include_directories(${CMAKE_SOURCE_DIR}/tools/frameworks/googletest/include)
include_directories(${CMAKE_BINARY_DIR}/include/)

include_directories(${COMMON_INCLUDE_DIRS})
if (${USE_GLES2})
include_directories(${GLES2_INCLUDE_DIRS})
elseif (${USE_VULKAN})
include_directories(${VULKAN_INCLUDE_DIRS})
elseif (${USE_SOFTWARE})
include_directories(${SOFTWARE_INCLUDE_DIRS})
endif()

add_executable(unit EXCLUDE_FROM_ALL ${SOURCES})
add_dependencies(unit gtest)
target_link_libraries(unit gepard X11 ${CMAKE_SOURCE_DIR}/tools/frameworks/googletest/lib/libgtest.a pthread ${LIB_DEPENDENCIES})
