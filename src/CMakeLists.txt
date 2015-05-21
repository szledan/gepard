file(GLOB SOURCES *.cpp)

add_library(gepard SHARED ${SOURCES})
#set_target_properties(gepard PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})

file(COPY gepard.h DESTINATION ${INCLUDE_OUTPUT_DIR})
