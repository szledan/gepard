SET(GEPARD_DEP_LIBS "")
SET(GEPARD_DEP_INCLUDES "")

if (BACKEND STREQUAL "GLES2")
  find_package(GLESv2 REQUIRED)
  find_package(EGL REQUIRED)

  list(APPEND GEPARD_DEP_LIBS ${GLESv2_LIBRARY} ${EGL_LIBRARY})
  list(APPEND GEPARD_DEP_INCLUDES ${GLESv2_INCLUDE_DIR} ${EGL_INCLUDE_DIR})
elseif (BACKEND STREQUAL "VULKAN")
  find_package(Vulkan)
  if (NOT VULKAN_FOUND)
    message(STATUS "Vulkan headers will be downloaded in a build step")
    add_custom_command (OUTPUT ${PROJECT_BINARY_DIR}/thirdparty/include/vulkan/vulkan.h
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

    set(VULKAN_INCLUDE_DIR ${PROJECT_BINARY_DIR}/thirdparty/include)
  endif()

  # TODO(kkristof) remove this once XSync has been removed from GepardVulkan::createSwapChain
  find_package(X11)
  list(APPEND GEPARD_DEP_LIBS ${X11_LIBRARIES})

  list(APPEND GEPARD_DEP_INCLUDES ${VULKAN_INCLUDE_DIR})
  list(APPEND GEPARD_DEP_LIBS ${CMAKE_DL_LIBS})
endif ()
