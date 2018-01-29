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

if (BACKEND STREQUAL "VULKAN" AND NOT VULKAN_FOUND)
  message(STATUS "Adding target to download Vulkan headers")
  add_custom_command (OUTPUT ${PROJECT_BINARY_DIR}/third-party/include/vulkan/vulkan.h
                      COMMAND mkdir -p "${PROJECT_BINARY_DIR}/thirdparty/include/vulkan"
                      COMMAND wget -O "${PROJECT_BINARY_DIR}/thirdparty/include/vulkan/vulkan.h"
                                      "https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/1.0/src/vulkan/vulkan.h")

  add_custom_command (OUTPUT ${PROJECT_BINARY_DIR}/thirdparty/include/vulkan/vk_platform.h
                      COMMAND mkdir -p "${PROJECT_BINARY_DIR}/thirdparty/include/vulkan"
                      COMMAND wget -O "${PROJECT_BINARY_DIR}/thirdparty/include/vulkan/vk_platform.h"
                                      "https://raw.githubusercontent.com/KhronosGroup/Vulkan-Docs/1.0/src/vulkan/vk_platform.h")

  add_custom_target(vulkan_headers
                    DEPENDS ${PROJECT_BINARY_DIR}/thirdparty/include/vulkan/vulkan.h
                            ${PROJECT_BINARY_DIR}/thirdparty/include/vulkan/vk_platform.h)

  add_dependencies(gepard vulkan_headers)
  target_include_directories(gepard PUBLIC ${PROJECT_BINARY_DIR}/thirdparty/include)
endif()

target_compile_definitions(gepard PUBLIC "GD_USE_${BACKEND}")
target_include_directories(gepard PUBLIC ${COMMON_INCLUDE_DIRS})
target_include_directories(gepard PUBLIC ${${BACKEND}_INCLUDE_DIRS})

# TODO(dbatyai): Add a target to do this instead of doing it at configure time
file(COPY gepard.h DESTINATION ${INCLUDE_OUTPUT_DIR})
file(COPY surfaces DESTINATION ${INCLUDE_OUTPUT_DIR})
