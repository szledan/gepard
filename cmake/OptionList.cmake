include(OptionMacros)

ADD_OPTION (USE_GLES2 "Use GLES 2.0" ON)
ADD_OPTION (USE_VULKAN "Use Vulkan" OFF)
ADD_OPTION (LOG_LEVEL "Print log messages during execution" 0)
ADD_OPTION (DISABLE_LOG_COLORS "Do not color log messages" OFF)

PROCESS_OPTIONS()
