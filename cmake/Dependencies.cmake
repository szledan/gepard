SET(LIB_DEPENDENCIES "")

find_package(EGL REQUIRED)
if (${USE_GLESv2})
  find_package(GLESv2 REQUIRED)
endif ()
