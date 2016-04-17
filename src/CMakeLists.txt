file(GLOB SOURCES *.cpp gpu/*.cpp utils/*.cpp)

add_library(gepard SHARED ${SOURCES})
#set_target_properties(gepard PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})

include_directories(${CMAKE_SOURCE_DIR}/src/)
include_directories(${CMAKE_SOURCE_DIR}/src/gpu/)
include_directories(${CMAKE_SOURCE_DIR}/src/utils/)

file(COPY gepard.h DESTINATION ${INCLUDE_OUTPUT_DIR})
