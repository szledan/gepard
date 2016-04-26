file(GLOB NVIDIA_PATH "/usr/lib/nvidia-*")

# Check in CMAKE_LIBRARY_PATH, and in the nvidia library path
find_library(GLES2_LIBRARY GLESv2
             PATHS ${NVIDIA_PATH}
             NO_SYSTEM_ENVIRONMENT_PATH)

# If not found, check in other system directories
find_library(GLES2_LIBRARY GLESv2
             PATHS "/usr/lib/mesa")

find_path(GLES2_INCLUDE_DIR "GLES2/gl2.h")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLES2
                                  REQUIRED_VARS GLES2_LIBRARY GLES2_INCLUDE_DIR)

list(APPEND LIB_DEPENDENCIES ${GLES2_LIBRARY})
include_directories(${GLES2_INCLUDE_DIR})
