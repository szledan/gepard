find_path(VULKAN_INCLUDE_DIR "vulkan/vulkan.h")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Vulkan REQUIRED_VARS VULKAN_INCLUDE_DIR)

