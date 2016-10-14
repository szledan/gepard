SET(LIB_DEPENDENCIES "")

if (${USE_GLES2})
  find_package(GLES2 REQUIRED)
  find_package(EGL REQUIRED)
endif ()
