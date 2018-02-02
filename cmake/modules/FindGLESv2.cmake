find_library(GLESv2_LIBRARY GLESv2
             "/usr/lib/x86_64-linux-gnu")

find_path(GLESv2_INCLUDE_DIR "GLES2/gl2.h")

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLESv2
                                  REQUIRED_VARS GLESv2_LIBRARY GLESv2_INCLUDE_DIR)
