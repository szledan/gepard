file(GLOB NVIDIA_PATH "/usr/lib/nvidia-*")

# Check in CMAKE_LIBRARY_PATH, and in the nvidia library path
find_library(EGL_LIBRARY EGL
             PATHS ${NVIDIA_PATH}
             NO_SYSTEM_ENVIRONMENT_PATH)

# If not found, check in other system directories
find_library(EGL_LIBRARY EGL
             PATHS "/usr/lib/mesa")

find_path(EGL_INCLUDE_DIR "EGL/egl.h")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(EGL
                                  REQUIRED_VARS EGL_LIBRARY EGL_INCLUDE_DIR)

list(APPEND LIB_DEPENDENCIES ${EGL_LIBRARY})
include_directories(${EGL_INCLUDE_DIR})

