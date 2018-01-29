set(COMMON_SOURCES
    engines/gepard-context.cpp
    engines/gepard-path.cpp
    gepard.cpp
    gepard-engine.cpp
    utils/gepard-types.cpp
    utils/gepard-defs.cpp
)

set(GLES2_SOURCES
    engines/gles2/gepard-gles2.cpp
    engines/gles2/gepard-gles2-fill-path.cpp
    engines/gles2/gepard-gles2-fill-rect.cpp
    engines/gles2/gepard-gles2-shader-factory.cpp
    engines/gles2/gepard-gles2-stroke-builder.cpp
    engines/gles2/gepard-gles2-stroke-path.cpp
    engines/gles2/gepard-gles2-trapezoid-tessellator.cpp
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

if (BACKEND STREQUAL "GLES2")
  find_package(GLESv2 REQUIRED)
  find_package(EGL REQUIRED)
elseif (BACKEND STREQUAL "VULKAN")
  find_package(Vulkan REQUIRED)
endif ()

target_compile_definitions(gepard PUBLIC "GD_USE_${BACKEND}")
target_include_directories(gepard PUBLIC ${COMMON_INCLUDE_DIRS})
target_include_directories(gepard PUBLIC ${${BACKEND}_INCLUDE_DIRS})

# TODO(dbatyai): Find a better way to do this
file(COPY gepard.h DESTINATION ${INCLUDE_OUTPUT_DIR})
file(COPY surfaces DESTINATION ${INCLUDE_OUTPUT_DIR})
