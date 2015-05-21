file(GLOB SOURCES *.cpp)

add_library(gepard SHARED ${SOURCES})

file(COPY gepard.h DESTINATION ${CMAKE_BINARY_DIR}/src)
