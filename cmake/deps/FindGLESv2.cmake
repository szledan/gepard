file(GLOB NVIDIA_PATH "/usr/lib/nvidia-*")

find_library(GLESv2_LIBRARY GLESv2
             PATHS ${NVIDIA_PATH}
             NO_DEFAULT_PATH)
find_library(GLESv2_LIBRARY GLESv2)

find_path(GLESv2_INCLUDE_DIR "GLES2/gl2.h")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLESv2
                                  REQUIRED_VARS GLESv2_LIBRARY GLESv2_INCLUDE_DIR)

list(APPEND LIB_DEPENDENCIES ${GLESv2_LIBRARY})
include_directories(${GLESv2_INCLUDE_DIR})
