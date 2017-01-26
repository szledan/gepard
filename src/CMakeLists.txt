file(GLOB SOURCES *.cpp)

add_library(gepard SHARED ${SOURCES})
#set_target_properties(gepard PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${LIB_OUTPUT_DIR})

file(COPY
 gepard.h
 config.h
 gepard-defs.h
 gepard-path.h
 gepard-surface.h
 gepard-texture.h
 gepard-trapezoid-tessellator.h
 gepard-types.h
 gepard-utils.h
 DESTINATION ${INCLUDE_OUTPUT_DIR})
