
find_library(EGL_LIBRARY EGL
             PATHS "/usr/lib/x86_64-linux-gnu")

find_path(EGL_INCLUDE_DIR "EGL/egl.h")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(EGL
                                  REQUIRED_VARS EGL_LIBRARY EGL_INCLUDE_DIR)
