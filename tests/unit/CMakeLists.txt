include(ExternalProject)

set(EXTRA_FLAGS " -Wall -pedantic -std=c++11 ")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_FLAGS}")

include_directories(${CMAKE_BINARY_DIR}/include/)

file(GLOB_RECURSE header_list ${CMAKE_SOURCE_DIR}/src/*.h)
set(dir_list "")
foreach(file_path ${header_list})
  get_filename_component(dir_path ${file_path} PATH)
  list(APPEND dir_list ${dir_path})
endforeach()
list(REMOVE_DUPLICATES dir_list)
include_directories(${dir_list})

ExternalProject_Add(gtest
                    PREFIX ${CMAKE_SOURCE_DIR}/tools/frameworks/googletest
                    GIT_REPOSITORY https://github.com/google/googletest.git
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/tools/frameworks/googletest
                    INSTALL_COMMAND make install > /dev/null)
set_target_properties(gtest PROPERTIES EXCLUDE_FROM_ALL TRUE)

include_directories(${CMAKE_SOURCE_DIR}/tools/frameworks/googletest/include)

file(GLOB SOURCES *.cpp)
if( SOURCES )
  add_executable(unit EXCLUDE_FROM_ALL ${SOURCES})
  add_dependencies(unit gtest)
  target_link_libraries(unit X11 ${CMAKE_SOURCE_DIR}/tools/frameworks/googletest/lib/libgtest.a pthread ${LIB_DEPENDENCIES})
endif()
