set(COMMON_SOURCES
    engines/gepard-context.cpp
    engines/gepard-path.cpp
    engines/gepard-stroke-builder.cpp
    engines/gepard-trapezoid-tessellator.cpp
    gepard.cpp
    gepard-engine.cpp
    gepard-image.cpp
    utils/gepard-bounding-box.cpp
    utils/gepard-color.cpp
    utils/gepard-defs.cpp
    utils/gepard-float-point.cpp
    utils/gepard-line-types.cpp
    utils/gepard-transform.cpp
    utils/gepard-vec4.cpp
)

set(GLES2_SOURCES
    engines/gles2/gepard-gles2.cpp
    engines/gles2/gepard-gles2-fill-path.cpp
    engines/gles2/gepard-gles2-fill-rect.cpp
    engines/gles2/gepard-gles2-shader-factory.cpp
    engines/gles2/gepard-gles2-stroke-path.cpp
)

set(VULKAN_SOURCES
    engines/vulkan/gepard-vulkan-interface.cpp
    engines/vulkan/gepard-vulkan.cpp
)

set(SOFTWARE_SOURCES
    engines/software/gepard-software.cpp
)

set(COMMON_INCLUDE_DIRS
    .
    engines
    surfaces
    utils
)

set(GLES2_INCLUDE_DIRS
    engines/gles2
)

set(VULKAN_INCLUDE_DIRS
    engines/vulkan
    engines/vulkan/shaders
)

set(SOFTWARE_INCLUDE_DIRS
    engines/software
)

set(GEPARD_HEADERS
    gepard.h
    gepard-image.h
)

set(SURFACE_HEADERS
    surfaces/gepard-png-surface.h
    surfaces/gepard-xsurface.h
    surfaces/gepard-memory-buffer-surface.h
)

set(SOURCES ${COMMON_SOURCES})
list(APPEND SOURCES ${${BACKEND}_SOURCES})

add_library(gepard SHARED ${SOURCES})

if (BACKEND STREQUAL "VULKAN" AND NOT VULKAN_FOUND)
  add_dependencies(gepard vulkan_headers)
endif()

if (BACKEND STREQUAL "VULKAN")
  add_dependencies(gepard vulkan_shaders)
endif()

target_compile_definitions(gepard PRIVATE "GD_USE_${BACKEND}")

target_include_directories(gepard PRIVATE ${COMMON_INCLUDE_DIRS})
target_include_directories(gepard PRIVATE ${${BACKEND}_INCLUDE_DIRS})
target_include_directories(gepard PRIVATE ${GEPARD_DEP_INCLUDES})

# TODO(dbatyai): Add a target to do this instead of doing it at configure time
file(COPY gepard.h DESTINATION ${INCLUDE_OUTPUT_DIR})
file(COPY gepard-image.h DESTINATION ${INCLUDE_OUTPUT_DIR})
file(COPY surfaces DESTINATION ${INCLUDE_OUTPUT_DIR})

set_target_properties(gepard
                      PROPERTIES
                      FRAMEWORK ON
                      VERSION ${PROJECT_VERSION}
                      PUBLIC_HEADER "${GEPARD_HEADERS}"
                      RESOURCE "${SURFACE_HEADERS}"
                      )

install(TARGETS gepard
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        RESOURCE DESTINATION ${CMAKE_INSTALL_PREFIX}/include/surfaces
        )
