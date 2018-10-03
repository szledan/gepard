set(VK_SHADER_DIRECTORY ${PROJECT_SOURCE_DIR}/src/engines/vulkan/shaders)
set(SPIRV_DIRECTORY ${PROJECT_BINARY_DIR}/spirv)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/spirv)

add_custom_target(
    vulkan_shaders
    DEPENDS ${SPIRV_DIRECTORY}/fill-rect.vert.inc
            ${SPIRV_DIRECTORY}/fill-rect.frag.inc
            ${SPIRV_DIRECTORY}/image.vert.inc
            ${SPIRV_DIRECTORY}/image.frag.inc
)

add_custom_command(
    OUTPUT ${SPIRV_DIRECTORY}/fill-rect.vert.inc
    COMMAND ${GLSLANG_VALIDATOR} -V -x -o ${SPIRV_DIRECTORY}/fill-rect.vert.inc ${VK_SHADER_DIRECTORY}/fill-rect.vert
    DEPENDS ${VK_SHADER_DIRECTORY}/fill-rect.vert ${GLSLANG_VALIDATOR}
)

add_custom_command(
    OUTPUT ${SPIRV_DIRECTORY}/fill-rect.frag.inc
    COMMAND ${GLSLANG_VALIDATOR} -V -x -o ${SPIRV_DIRECTORY}/fill-rect.frag.inc ${VK_SHADER_DIRECTORY}/fill-rect.frag
    DEPENDS ${VK_SHADER_DIRECTORY}/fill-rect.frag ${GLSLANG_VALIDATOR}
)

add_custom_command(
    OUTPUT ${SPIRV_DIRECTORY}/image.vert.inc
    COMMAND ${GLSLANG_VALIDATOR} -V -x -o ${SPIRV_DIRECTORY}/image.vert.inc ${VK_SHADER_DIRECTORY}/image.vert
    DEPENDS ${VK_SHADER_DIRECTORY}/image.vert ${GLSLANG_VALIDATOR}
)

add_custom_command(
    OUTPUT ${SPIRV_DIRECTORY}/image.frag.inc
    COMMAND ${GLSLANG_VALIDATOR} -V -x -o ${SPIRV_DIRECTORY}/image.frag.inc ${VK_SHADER_DIRECTORY}/image.frag
    DEPENDS ${VK_SHADER_DIRECTORY}/image.frag ${GLSLANG_VALIDATOR}
)
