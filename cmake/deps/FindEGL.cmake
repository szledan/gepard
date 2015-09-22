file(GLOB NVIDIA_PATH "/usr/lib/nvidia-*")

find_library(EGL_LIBRARY EGL
             PATHS ${NVIDIA_PATH}
             NO_DEFAULT_PATH)
find_library(EGL_LIBRARY EGL)

find_path(EGL_INCLUDE_DIR "EGL/egl.h")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(EGL
                                  REQUIRED_VARS EGL_LIBRARY EGL_INCLUDE_DIR)

list(APPEND LIB_DEPENDENCIES ${EGL_LIBRARY})
include_directories(${EGL_INCLUDE_DIR})

