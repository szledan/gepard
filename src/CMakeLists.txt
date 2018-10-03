set(COMMON_SOURCES
    engines/gepard-context.cpp
    engines/gepard-path.cpp
    engines/gepard-stroke-builder.cpp
    engines/gepard-trapezoid-tessellator.cpp
    gepard.cpp
    gepard-engine.cpp
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
)

set(SOFTWARE_INCLUDE_DIRS
    engines/software
)

set(SOURCES ${COMMON_SOURCES})
list(APPEND SOURCES ${${BACKEND}_SOURCES})

add_library(gepard SHARED ${SOURCES})

if (BACKEND STREQUAL "VULKAN" AND NOT VULKAN_FOUND)
  add_dependencies(gepard vulkan_headers)
endif()

target_compile_definitions(gepard PUBLIC "GD_USE_${BACKEND}")

target_include_directories(gepard PUBLIC ${COMMON_INCLUDE_DIRS})
target_include_directories(gepard PUBLIC ${${BACKEND}_INCLUDE_DIRS})
target_include_directories(gepard PUBLIC ${GEPARD_DEP_INCLUDES})

# TODO(dbatyai): Add a target to do this instead of doing it at configure time
file(COPY gepard.h DESTINATION ${INCLUDE_OUTPUT_DIR})
file(COPY surfaces DESTINATION ${INCLUDE_OUTPUT_DIR})
