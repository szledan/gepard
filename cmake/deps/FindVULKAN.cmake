file(GLOB NVIDIA_PATH "/usr/lib/nvidia-*")

find_path(VULKAN_INCLUDE_DIR "vulkan/vulkan.h"
          PATH "thirdparty")

#find_library(VULKAN_LIBRARY vulkan
#             PATHS ${NVIDIA_PATH}
#             NO_DEFAULT_PATH)
#find_library(VULKAN_LIBRARY vulkan)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(VULKAN
                                  REQUIRED_VARS VULKAN_INCLUDE_DIR)

include_directories(${VULKAN_INCLUDE_DIR})

