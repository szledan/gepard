/* Copyright (C) 2016, Gepard Graphics
 * Copyright (C) 2016, Kristof Kosztyo <kkristof@inf.u-szeged.hu>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef GD_USE_VULKAN

#include "gepard-vulkan.h"

#include <cstring>
#include <fstream>
#include <vector>

#include <png.h>

namespace gepard {
namespace vulkan {

GepardVulkan::GepardVulkan(Surface* surface)
    : _surface(surface)
    , _vk("libvulkan.so")
    , _allocator(nullptr)
    , _instance(0)
    , _imageFormat(VK_FORMAT_R8G8B8A8_UNORM)
{
    GD_LOG1("GepardVulkan");
    _vk.loadGlobalFunctions();
    GD_LOG2(" - Global functions are loaded");
    createDefaultInstance();
    _vk.loadInstanceFunctions(_instance);
    GD_LOG2(" - Instance functions are loaded");
    chooseDefaultDevice();
    _vk.loadDeviceFunctions(_device);
    GD_LOG2(" - Device functions are loaded");
    createCommandPool();
    allocatePrimaryCommandBuffer();
    GD_LOG2(" - Command buffer is allocated");
    createDefaultRenderPass();
    GD_LOG2(" - Default render pass is created");
    createSurfaceImage();
    GD_LOG2(" - Surface backing image is created");
    createDefaultFrameBuffer();
    GD_LOG2(" - Default frame buffer is created");
}

GepardVulkan::~GepardVulkan()
{
    // TODO: it would be extremely usefull to have container classes for these
    if (_frameBuffer) {
        _vk.vkDestroyFramebuffer(_device, _frameBuffer, _allocator);
    }
    if (_frameBufferColorAttachmentImageView) {
        _vk.vkDestroyImageView(_device, _frameBufferColorAttachmentImageView, _allocator);
    }
    if (_surfaceImage) {
        _vk.vkDestroyImage(_device, _surfaceImage, _allocator);
    }
    if (_renderPass) {
        _vk.vkDestroyRenderPass(_device, _renderPass, _allocator);
    }
    if (_secondaryCommandBuffers.size()) {
        _vk.vkFreeCommandBuffers(_device, _commandPool, _secondaryCommandBuffers.size(), _secondaryCommandBuffers.data());
    }
    if (_primaryCommandBuffers.size()) {
        _vk.vkFreeCommandBuffers(_device, _commandPool, _primaryCommandBuffers.size(), _primaryCommandBuffers.data());
    }
    for (uint32_t i = 0; i < _memoryAllocations.size(); i++) {
        _vk.vkFreeMemory(_device, _memoryAllocations[i], _allocator);
    }
    if (_commandPool) {
        _vk.vkDestroyCommandPool(_device, _commandPool, _allocator);
    }
    if (_device) {
        _vk.vkDestroyDevice(_device, _allocator);
    }
    if (_instance) {
        _vk.vkDestroyInstance(_instance, _allocator);
    }
}

void GepardVulkan::fillRect(Float x, Float y, Float w, Float h)
{
    GD_LOG_FUNC(" (" << x << ", " << y << ", " << w << ", " << h << " )" );

    // Vertex data setup

    static int index = 0;
    float green = 0.2 * index++;
    // Todo: use the actual state as color
    float color[] = { 0.5, green, 0.0, 1.0 };

    float left = (float)((2.0 * x / (float)_surface->width()) - 1.0);
    float right = (float)((2.0 * (x + w) / (float)_surface->width()) - 1.0);
    float top = (float)((2.0 * y / (float)_surface->height()) - 1.0);
    float bottom = (float)((2.0 * (y + h) / (float)_surface->height()) - 1.0);

    float vertexData[] = {
        left, top, 1.0, 1.0, color[0], color[1], color[2], color[3],
        right, top, 1.0, 1.0, color[0], color[1], color[2], color[3],
        left, bottom, 1.0, 1.0, color[0], color[1], color[2], color[3],
        right, bottom, 1.0, 1.0, color[0], color[1], color[2], color[3],
    };

    uint32_t rectIndicies[] = {0, 1, 2, 2, 1, 3};

    VkBufferCreateInfo vertexBufferInfo = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,   // VkStructureType        sType;
        nullptr,                                // const void*            pNext;
        0,                                      // VkBufferCreateFlags    flags;
        (VkDeviceSize)sizeof(vertexData),       // VkDeviceSize           size;
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,      // VkBufferUsageFlags     usage;
        VK_SHARING_MODE_EXCLUSIVE,              // VkSharingMode          sharingMode;
        1u,                                     // uint32_t               queueFamilyIndexCount;
        &_queueFamilyIndex,                     // const uint32_t*        pQueueFamilyIndices;
    };

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    VkDeviceSize vertexBufferOffset = 0;

    _vk.vkCreateBuffer(_device, &vertexBufferInfo, _allocator, &vertexBuffer);

    {
        VkMemoryRequirements memoryRequirements;
        _vk.vkGetBufferMemoryRequirements(_device, vertexBuffer, &memoryRequirements);
        VkMemoryAllocateInfo allocationInfo = {
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,                                         // VkStructureType    sType;
            nullptr,                                                                        // const void*        pNext;
            memoryRequirements.size,                                                        // VkDeviceSize       allocationSize;
            getMemoryTypeIndex(memoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT),    // uint32_t           memoryTypeIndex;
        };

        _vk.vkAllocateMemory(_device, &allocationInfo, _allocator, &vertexBufferMemory);
        _vk.vkBindBufferMemory(_device, vertexBuffer, vertexBufferMemory, vertexBufferOffset);

        void* data;
        _vk.vkMapMemory(_device, vertexBufferMemory, 0, (VkDeviceSize)sizeof(vertexData), 0, &data);

        std::memcpy(data, vertexData, (VkDeviceSize)sizeof(vertexData));

        VkMappedMemoryRange range = {
            VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // VkStructureType    sType;
            nullptr,                                // const void*        pNext;
            vertexBufferMemory,                     // VkDeviceMemory     memory;
            0,                                      // VkDeviceSize       offset;
            (VkDeviceSize)sizeof(vertexData),       // VkDeviceSize       size;
        };

        _vk.vkFlushMappedMemoryRanges(_device, 1, &range);
        _vk.vkUnmapMemory(_device, vertexBufferMemory);
    }

    VkBufferCreateInfo indexBufferInfo = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,   // VkStructureType        sType;
        nullptr,                                // const void*            pNext;
        0,                                      // VkBufferCreateFlags    flags;
        (VkDeviceSize)sizeof(rectIndicies),     // VkDeviceSize           size;
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,       // VkBufferUsageFlags     usage;
        VK_SHARING_MODE_EXCLUSIVE,              // VkSharingMode          sharingMode;
        1u,                                     // uint32_t               queueFamilyIndexCount;
        &_queueFamilyIndex,                      // const uint32_t*        pQueueFamilyIndices;
    };

    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;

    _vk.vkCreateBuffer(_device, &indexBufferInfo, _allocator, &indexBuffer);

    {
        VkMemoryRequirements memoryRequirements;
        _vk.vkGetBufferMemoryRequirements(_device, indexBuffer, &memoryRequirements);
        VkMemoryAllocateInfo allocationInfo = {
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,                                         // VkStructureType    sType;
            nullptr,                                                                        // const void*        pNext;
            memoryRequirements.size,                                                        // VkDeviceSize       allocationSize;
            getMemoryTypeIndex(memoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT),    // uint32_t           memoryTypeIndex;
        };

        _vk.vkAllocateMemory(_device, &allocationInfo, _allocator, &indexBufferMemory);
        _vk.vkBindBufferMemory(_device, indexBuffer, indexBufferMemory, 0);

        void* data;
        _vk.vkMapMemory(_device, indexBufferMemory, 0, (VkDeviceSize)sizeof(rectIndicies), 0, &data);

        std::memcpy(data, rectIndicies, (VkDeviceSize)sizeof(rectIndicies));

        VkMappedMemoryRange range = {
            VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // VkStructureType    sType;
            nullptr,                                // const void*        pNext;
            indexBufferMemory,                      // VkDeviceMemory     memory;
            0,                                      // VkDeviceSize       offset;
            (VkDeviceSize)sizeof(rectIndicies),     // VkDeviceSize       size;
        };

        _vk.vkFlushMappedMemoryRanges(_device, 1, &range);
        _vk.vkUnmapMemory(_device, indexBufferMemory);
    }

    // Pipeline creation

    VkShaderModule vertex;
    VkShaderModule fragment;

    // TODO: better file handling is needed.
    {
        std::ifstream vertexInput( "src/engines/vulkan/shaders/vert.spv", std::ios::binary );
        std::vector<char> vertexCode((std::istreambuf_iterator<char>(vertexInput)), (std::istreambuf_iterator<char>()));

        std::ifstream fragmentInput( "src/engines/vulkan/shaders/frag.spv", std::ios::binary );
        std::vector<char> fragmentCode((std::istreambuf_iterator<char>(fragmentInput)), (std::istreambuf_iterator<char>()));

        VkShaderModuleCreateInfo vertexModulInfo = {
            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,            // VkStructureType              sType;
            nullptr,                                                // const void*                  pNext;
            0,                                                      // VkShaderModuleCreateFlags    flags;
            vertexCode.size(),                                      // size_t                       codeSize;
            reinterpret_cast<const uint32_t*>(vertexCode.data()),   // const uint32_t*              pCode;
        };
        _vk.vkCreateShaderModule(_device, &vertexModulInfo, _allocator, &vertex);

        VkShaderModuleCreateInfo fragmentModulInfo = {
            VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,            // VkStructureType              sType;
            nullptr,                                                // const void*                  pNext;
            0,                                                      // VkShaderModuleCreateFlags    flags;
            fragmentCode.size(),                                    // size_t                       codeSize;
            reinterpret_cast<const uint32_t*>(fragmentCode.data()), // const uint32_t*              pCode;
        };
        _vk.vkCreateShaderModule(_device, &fragmentModulInfo, _allocator, &fragment);
    }

    VkPipelineShaderStageCreateInfo stages[] = {
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,    // VkStructureType                     sType;
            nullptr,                                                // const void*                         pNext;
            0,                                                      // VkPipelineShaderStageCreateFlags    flags;
            VK_SHADER_STAGE_VERTEX_BIT,                             // VkShaderStageFlagBits               stage;
            vertex,                                                 // VkShaderModule                      module;
            "main",                                                 // const char*                         pName;
            nullptr,                                                // const VkSpecializationInfo*         pSpecializationInfo;
        },
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,    // VkStructureType                     sType;
            nullptr,                                                // const void*                         pNext;
            0,                                                      // VkPipelineShaderStageCreateFlags    flags;
            VK_SHADER_STAGE_FRAGMENT_BIT,                           // VkShaderStageFlagBits               stage;
            fragment,                                               // VkShaderModule                      module;
            "main",                                                 // const char*                         pName;
            nullptr,                                                // const VkSpecializationInfo*         pSpecializationInfo;
        }
    };

    VkVertexInputBindingDescription bindingDescription = {
        0u,                             // uint32_t             binding;
        2 * (4 * sizeof(float)),        // uint32_t             stride;
        VK_VERTEX_INPUT_RATE_VERTEX,    // VkVertexInputRate    inputRate;
    };

    const VkVertexInputAttributeDescription vertexAttributeDescriptions[] = {
        {
            0u,                             // uint32_t location
            0u,                             // uint32_t binding
            VK_FORMAT_R32G32B32A32_SFLOAT,  // VkFormat format
            0u,                             // uint32_t offset
        },
        {
            1u,                             // uint32_t location
            0u,                             // uint32_t binding
            VK_FORMAT_R32G32B32A32_SFLOAT,  // VkFormat format
            sizeof(float) * 4,              // uint32_t offset
        },
    };

    VkPipelineVertexInputStateCreateInfo vertexInputState = {
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,  // VkStructureType                             sType;
        nullptr,                                                    // const void*                                 pNext;
        0,                                                          // VkPipelineVertexInputStateCreateFlags       flags;
        1u,                                                         // uint32_t                                    vertexBindingDescriptionCount;
        &bindingDescription,                                        // const VkVertexInputBindingDescription*      pVertexBindingDescriptions;
        2u,                                                         // uint32_t                                    vertexAttributeDescriptionCount;
        vertexAttributeDescriptions,                                // const VkVertexInputAttributeDescription*    pVertexAttributeDescriptions;
    };

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,    // VkStructureType                          sType
        nullptr,                                                        // const void*                              pNext
        0,                                                              // VkPipelineInputAssemblyStateCreateFlags  flags
        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,                            // VkPrimitiveTopology                      topology
        VK_FALSE,                                                       // VkBool32                                 primitiveRestartEnable
    };

    const VkViewport viewports[] = {
        {
            0.0f,                       // float    x;
            0.0f,                       // float    y;
            (float)_surface->width(),   // float    width;
            (float)_surface->height(),  // float    height;
            0.0f,                       // float    minDepth;
            1.0f,                       // float    maxDepth;
        }
    };

    const VkRect2D scissors[] = {
        {
            {
                0,  // int32_t x
                0,  // int32_t y
            },  // VkOffset2D    offset;
            {
                _surface->width(),  // uint32_t    width
                _surface->height(), // uint32_t    height
            }, // VkExtent2D    extent;
        }
    };

    VkPipelineViewportStateCreateInfo viewportState = {
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,  // VkStructureType                       sType;
        nullptr,                                                // const void*                           pNext;
        0,                                                      // VkPipelineViewportStateCreateFlags    flags;
        1u,                                                     // uint32_t                              viewportCount;
        viewports,                                              // const VkViewport*                     pViewports;
        1u,                                                     // uint32_t                              scissorCount;
        scissors,                                               // const VkRect2D*                       pScissors;
    };

    VkPipelineRasterizationStateCreateInfo rasterizationState = {
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO, // VkStructureType                            sType;
        nullptr,                                                    // const void*                                pNext;
        0,                                                          // VkPipelineRasterizationStateCreateFlags    flags;
        VK_FALSE,                                                   // VkBool32                                   depthClampEnable;
        VK_FALSE,                                                   // VkBool32                                   rasterizerDiscardEnable;
        VK_POLYGON_MODE_FILL,                                       // VkPolygonMode                              polygonMode;
        VK_CULL_MODE_NONE,                                          // VkCullModeFlags                            cullMode;
        VK_FRONT_FACE_COUNTER_CLOCKWISE,                            // VkFrontFace                                frontFace;
        VK_FALSE,                                                   // VkBool32                                   depthBiasEnable;
        0.0f,                                                       // float                                      depthBiasConstantFactor;
        0.0f,                                                       // float                                      depthBiasClamp;
        0.0f,                                                       // float                                      depthBiasSlopeFactor;
        1.0f,                                                       // float                                      lineWidth;
    };

    VkPipelineMultisampleStateCreateInfo multisampleState = {
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,   // VkStructureType                          sType;
        nullptr,                                                    // const void*                              pNext;
        0,                                                          // VkPipelineMultisampleStateCreateFlags    flags;
        VK_SAMPLE_COUNT_1_BIT,                                      // VkSampleCountFlagBits                    rasterizationSamples;
        VK_FALSE,                                                   // VkBool32                                 sampleShadingEnable;
        0.0,                                                        // float                                    minSampleShading;
        nullptr,                                                    // const VkSampleMask*                      pSampleMask;
        VK_FALSE,                                                   // VkBool32                                 alphaToCoverageEnable;
        VK_FALSE,                                                   // VkBool32                                 alphaToOneEnable;
    };

    VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
        VK_TRUE,                                                                                                    // VkBool32                 blendEnable;
        VK_BLEND_FACTOR_ONE,                                                                                        // VkBlendFactor            srcColorBlendFactor;
        VK_BLEND_FACTOR_ZERO,                                                                                       // VkBlendFactor            dstColorBlendFactor;
        VK_BLEND_OP_ADD,                                                                                            // VkBlendOp                colorBlendOp;
        VK_BLEND_FACTOR_ONE,                                                                                        // VkBlendFactor            srcAlphaBlendFactor;
        VK_BLEND_FACTOR_ZERO,                                                                                       // VkBlendFactor            dstAlphaBlendFactor;
        VK_BLEND_OP_ADD,                                                                                            // VkBlendOp                alphaBlendOp;
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT   // VkColorComponentFlags    colorWriteMask;
    };

    VkPipelineColorBlendStateCreateInfo colorBlendState = {
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,   // VkStructureType                               sType;
        nullptr,                                                    // const void*                                   pNext;
        0,                                                          // VkPipelineColorBlendStateCreateFlags          flags;
        VK_FALSE,                                                   // VkBool32                                      logicOpEnable;
        VK_LOGIC_OP_COPY,                                           // VkLogicOp                                     logicOp;
        1u,                                                         // uint32_t                                      attachmentCount;
        &color_blend_attachment_state,                              // const VkPipelineColorBlendAttachmentState*    pAttachments;
        {
            0.0f,   // float R
            0.0f,   // float G
            0.0f,   // float B
            0.0f,   // float A
        },                                                          // float                                         blendConstants[4];
    };

    VkPipelineLayout layout;
    VkPipelineLayoutCreateInfo layoutCreateInfo = {
          VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,  // VkStructureType                sType
          nullptr,                                        // const void                    *pNext
          0,                                              // VkPipelineLayoutCreateFlags    flags
          0,                                              // uint32_t                       setLayoutCount
          nullptr,                                        // const VkDescriptorSetLayout   *pSetLayouts
          0,                                              // uint32_t                       pushConstantRangeCount
          nullptr                                         // const VkPushConstantRange     *pPushConstantRanges
    };

    _vk.vkCreatePipelineLayout(_device, &layoutCreateInfo, _allocator, &layout);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
        VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,    // VkStructureType                                  sType;
        nullptr,                                            // const void*                                      pNext;
        0,                                                  // VkPipelineCreateFlags                            flags;
        2u,                                                 // uint32_t                                         stageCount;
        stages,                                             // const VkPipelineShaderStageCreateInfo*           pStages;
        &vertexInputState,                                  // const VkPipelineVertexInputStateCreateInfo*      pVertexInputState;
        &inputAssemblyState,                                // const VkPipelineInputAssemblyStateCreateInfo*    pInputAssemblyState;
        nullptr,                                            // const VkPipelineTessellationStateCreateInfo*     pTessellationState;
        &viewportState,                                     // const VkPipelineViewportStateCreateInfo*         pViewportState;
        &rasterizationState,                                // const VkPipelineRasterizationStateCreateInfo*    pRasterizationState;
        &multisampleState,                                  // const VkPipelineMultisampleStateCreateInfo*      pMultisampleState;
        nullptr,                                            // const VkPipelineDepthStencilStateCreateInfo*     pDepthStencilState;
        &colorBlendState,                                   // const VkPipelineColorBlendStateCreateInfo*       pColorBlendState;
        nullptr,                                            // const VkPipelineDynamicStateCreateInfo*          pDynamicState;
        layout,                                             // VkPipelineLayout                                 layout;
        _renderPass,                                        // VkRenderPass                                     renderPass;
        0u,                                                 // uint32_t                                         subpass;
        VK_NULL_HANDLE,                                     // VkPipeline                                       basePipelineHandle;
        0,                                                 // int32_t                                          basePipelineIndex;
    };

    VkPipeline pipeline;

    _vk.vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, _allocator, &pipeline);

    // Drawing

    VkCommandBuffer commandBuffer = _primaryCommandBuffers[0];

    VkCommandBufferBeginInfo commandBufferBeginInfo = {
       VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO, // VkStructureType                          sType;
       nullptr,                                     // const void*                              pNext;
       VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, // VkCommandBufferUsageFlags                flags;
       nullptr,                                     // const VkCommandBufferInheritanceInfo*    pInheritanceInfo;
    };

    _vk.vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

    VkClearValue clearValue = { 0.0, 0.0, 0.0, 0.0 };

    VkRenderPassBeginInfo renderPassInfo {
        VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,   // VkStructureType        sType;
        nullptr,                                    // const void*            pNext;
        _renderPass,                                // VkRenderPass           renderPass;
        _frameBuffer,                               // VkFramebuffer          framebuffer;
        scissors[0],                                // VkRect2D               renderArea;
        1,                                          // uint32_t               clearValueCount;
        &clearValue,                                // const VkClearValue*    pClearValues;
    };

    _vk.vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    _vk.vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    _vk.vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &vertexBufferOffset);
    _vk.vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

    uint32_t indexCount = sizeof(rectIndicies) / sizeof(uint32_t);
    _vk.vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);

    _vk.vkCmdEndRenderPass(commandBuffer);

    _vk.vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,  // VkStructureType                sType;
        nullptr,                        // const void*                    pNext;
        0,                              // uint32_t                       waitSemaphoreCount;
        nullptr,                        // const VkSemaphore*             pWaitSemaphores;
        nullptr,                        // const VkPipelineStageFlags*    pWaitDstStageMask;
        1,                              // uint32_t                       commandBufferCount;
        &commandBuffer,                 // const VkCommandBuffer*         pCommandBuffers;
        0,                              // uint32_t                       signalSemaphoreCount;
        nullptr,                        // const VkSemaphore*             pSignalSemaphores;
    };

    VkQueue queue;
    _vk.vkGetDeviceQueue(_device, _queueFamilyIndex, 0, &queue);

    VkFenceCreateInfo fenceInfo = {
        VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,    // VkStructureType       sType;
        nullptr,                                // const void*           pNext;
        0,                                      // VkFenceCreateFlags    flags;
    };

    VkFence fence;
    _vk.vkCreateFence(_device, &fenceInfo, _allocator, &fence);

    _vk.vkQueueSubmit(queue, 1, &submitInfo, fence);

    uint64_t timeout = (uint64_t)16 * 1000000; // 16 ms
    _vk.vkWaitForFences(_device, 1, &fence, VK_TRUE, timeout);

    // Clean up
    _vk.vkDestroyFence(_device, fence, _allocator);

    _vk.vkDestroyPipeline(_device, pipeline, _allocator);
    _vk.vkDestroyPipelineLayout(_device, layout, _allocator);

    _vk.vkDestroyShaderModule(_device, vertex, _allocator);
    _vk.vkDestroyShaderModule(_device, fragment, _allocator);

    _vk.vkDestroyBuffer(_device, vertexBuffer, _allocator);
    _vk.vkDestroyBuffer(_device, indexBuffer, _allocator);

    _vk.vkFreeMemory(_device, vertexBufferMemory, _allocator);
    _vk.vkFreeMemory(_device, indexBufferMemory, _allocator);

    VkExtent3D imageSize = {_surface->width(), _surface->height(), 1};
    logImage(_surfaceImage, imageSize);
}

void GepardVulkan::closePath()
{
    GD_LOG_FUNC(" ()");
}

void GepardVulkan::createDefaultInstance()
{
    GD_ASSERT(!_instance);

    // todo: find better default arguments
    const VkInstanceCreateInfo instanceCreateInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, // VkStructureType             sType;
        nullptr,                                // const void*                 pNext;
        0u,                                     // VkInstanceCreateFlags       flags;
        nullptr,                                // const VkApplicationInfo*    pApplicationInfo;
        0u,                                     // uint32_t                    enabledLayerCount;
        nullptr,                                // const char* const*          ppEnabledLayerNames;
        0u,                                     // uint32_t                    enabledExtensionCount;
        nullptr,                                // const char* const*          ppEnabledExtensionNames;
    };

    _vk.vkCreateInstance(&instanceCreateInfo, _allocator, &_instance);
}

void GepardVulkan::chooseDefaultPhysicalDevice()
{
    uint32_t deviceCount;
    _vk.vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr);
    GD_ASSERT(deviceCount);
    GD_LOG3("Physical devices found: " << deviceCount);
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    GD_ASSERT(deviceCount && "Couldn't find any device!");

    std::vector<VkPhysicalDevice> integratedDevices;
    std::vector<VkPhysicalDevice> discreteDevices;
    std::vector<VkPhysicalDevice> otherDevices;
    _vk.vkEnumeratePhysicalDevices(_instance, &deviceCount, physicalDevices.data());
    for (uint32_t i = 0; i < deviceCount; i++) {
           VkPhysicalDeviceProperties deviceProperties;
           _vk.vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
           GD_LOG3("device name " << deviceProperties.deviceName);
           GD_LOG3("api version " << (deviceProperties.apiVersion >> 22) << "."
               << ((deviceProperties.apiVersion >> 12) & 0x1ff) << "."  << (deviceProperties.apiVersion & 0x7ff));
           if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) {
               integratedDevices.push_back(physicalDevices[i]);
               continue;
           }
           else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
               discreteDevices.push_back(physicalDevices[i]);
               continue;
           }
           otherDevices.push_back(physicalDevices[i]);
    }

    if (findGraphicsQueue(discreteDevices))
        return;

    if (findGraphicsQueue(integratedDevices))
        return;

    if (findGraphicsQueue(otherDevices))
        return;

    GD_CRASH("Couldn't find any feasible vulkan device!");
}

bool GepardVulkan::findGraphicsQueue(std::vector<VkPhysicalDevice> devices)
{
    for (uint32_t i = 0; i < devices.size(); i++) {
        uint32_t queueCount;
        _vk.vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueCount);
        _vk.vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queueCount, queueFamilyProperties.data());
        for (uint32_t j = 0; j < queueCount; j++) {
            if (queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                _physicalDevice = devices[i];
                _queueFamilyIndex = j;
                return true;
            }
        }
    }

    return false;
}

void GepardVulkan::chooseDefaultDevice()
{
    chooseDefaultPhysicalDevice();
    _vk.vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &_physicalDeviceMemoryProperties);
    _vk.vkGetPhysicalDeviceFeatures(_physicalDevice, &_physicalDeviceFeatures);

    VkResult vkResult;
    const float queuePriorities[] = { 1.0f };

    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // VkStructureType             sType;
        nullptr,                                    // const void*                 pNext;
        0u,                                         // VkDeviceQueueCreateFlags    flags;
        _queueFamilyIndex,                          // uint32_t                    queueFamilyIndex;
        1u,                                         // uint32_t                    queueCount;
        queuePriorities                             // const float*                pQueuePriorities;
    };

    std::vector<const char*> enabledInstanceLayers;
    std::vector<const char*> enabledInstanceExtensions;

    enabledInstanceLayers.push_back("VK_LAYER_LUNARG_standard_validation");

    VkDeviceCreateInfo deviceCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,       // VkStructureType                  sType;
        nullptr,                                    // const void*                      pNext;
        0u,                                         // VkDeviceCreateFlags              flags;
        1u,                                         // uint32_t                         queueCreateInfoCount;
        &deviceQueueCreateInfo,                     // const VkDeviceQueueCreateInfo*   pQueueCreateInfos;
        (uint32_t)enabledInstanceLayers.size(),     // uint32_t                         enabledLayerCount;
        enabledInstanceLayers.data(),               // const char* const*               ppEnabledLayerNames;
        (uint32_t)enabledInstanceExtensions.size(), // uint32_t                         enabledExtensionCount;
        enabledInstanceExtensions.data(),           // const char* const*               ppEnabledExtensionNames;
        &_physicalDeviceFeatures,                    // const VkPhysicalDeviceFeatures*  pEnabledFeatures;
    };

    vkResult = _vk.vkCreateDevice(_physicalDevice, &deviceCreateInfo, _allocator, &_device);
    GD_ASSERT(vkResult == VK_SUCCESS && "Logical device creation is failed!");
}

void GepardVulkan::createCommandPool()
{
    VkCommandPoolCreateInfo commandPoolCreateInfo = {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,         // VkStructureType          sType;
        nullptr,                                            // const void*              pNext;
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,    // VkCommandPoolCreateFlags flags;
        _queueFamilyIndex,                                  // uint32_t                 queueFamilyIndex;
    };

    VkResult vkResult;
    vkResult = _vk.vkCreateCommandPool(_device, &commandPoolCreateInfo, _allocator, &_commandPool);

    GD_ASSERT(vkResult == VK_SUCCESS && "Command pool creation is failed!");
}

void GepardVulkan::allocatePrimaryCommandBuffer()
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO, // VkStructureType         sType;
        nullptr,                                        // const void*             pNext;
        _commandPool,                                   // VkCommandPool           commandPool;
        VK_COMMAND_BUFFER_LEVEL_PRIMARY,                // VkCommandBufferLevel    level;
        1,                                              // uint32_t                commandBufferCount;
    };

    VkResult vkResult;
    VkCommandBuffer commandBuffer;
    vkResult = _vk.vkAllocateCommandBuffers(_device, &commandBufferAllocateInfo, &commandBuffer);

    GD_ASSERT(vkResult == VK_SUCCESS && "Command buffer allocation is failed!");
    _primaryCommandBuffers.push_back(commandBuffer);
}

void GepardVulkan::createDefaultRenderPass()
{
    VkResult vkResult;

    VkAttachmentDescription attachmentDescription = {
        0u,                                         // VkAttachmentDescriptionFlags flags;
        _imageFormat,                               // VkFormat                     format;
        VK_SAMPLE_COUNT_1_BIT,                      // VkSampleCountFlagBits        samples;
        VK_ATTACHMENT_LOAD_OP_LOAD,            // VkAttachmentLoadOp           loadOp; TODO: change it back.
        VK_ATTACHMENT_STORE_OP_STORE,           // VkAttachmentStoreOp          storeOp;
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,            // VkAttachmentLoadOp           stencilLoadOp;
        VK_ATTACHMENT_STORE_OP_DONT_CARE,           // VkAttachmentStoreOp          stencilStoreOp;
        VK_IMAGE_LAYOUT_UNDEFINED,                  // VkImageLayout                initialLayout;
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,   // VkImageLayout                finalLayout;
    };

    VkAttachmentReference colorAttachment = {
        0u,                                         // uint32_t         attachment;
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,   // VkImageLayout    layout;
    };

    VkSubpassDescription subpassDescription = {
        0u,                                 // VkSubpassDescriptionFlags       flags;
        VK_PIPELINE_BIND_POINT_GRAPHICS,    // VkPipelineBindPoint             pipelineBindPoint;
        0u,                                 // uint32_t                        inputAttachmentCount;
        nullptr,                            // const VkAttachmentReference*    pInputAttachments;
        1u,                                 // uint32_t                        colorAttachmentCount;
        &colorAttachment,                   // const VkAttachmentReference*    pColorAttachments;
        nullptr,                            // const VkAttachmentReference*    pResolveAttachments;
        nullptr,                            // const VkAttachmentReference*    pDepthStencilAttachment;
        0u,                                 // uint32_t                        preserveAttachmentCount;
        nullptr,                            // const uint32_t*                 pPreserveAttachments;
    };

    VkRenderPassCreateInfo renderPassCreateInfo = {
        VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,  // VkStructureType                   sType;
        nullptr,                                    // const void*                       pNext;
        0u,                                         // VkRenderPassCreateFlags           flags;
        1u,                                         // uint32_t                          attachmentCount;
        &attachmentDescription,                     // const VkAttachmentDescription*    pAttachments;
        1u,                                         // uint32_t                          subpassCount;
        &subpassDescription,                        // const VkSubpassDescription*       pSubpasses;
        0u,                                         // uint32_t                          dependencyCount;
        nullptr,                                    // const VkSubpassDependency*        pDependencies;
    };

    vkResult = _vk.vkCreateRenderPass(_device, &renderPassCreateInfo, _allocator, &_renderPass);
    GD_ASSERT(vkResult == VK_SUCCESS && "Creating the default render pass is failed!");
}

void GepardVulkan::createSurfaceImage()
{
    VkResult vkResult;

    VkExtent3D imageSize = {
        _surface->width(),  // uint32_t    width;
        _surface->height(), // uint32_t    height;
        1,                  // uint32_t    depth;
    };

    VkImageCreateInfo imageCreateInfo = {
        VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,        // VkStructureType          sType;
        nullptr,                                    // const void*              pNext;
        0u,                                         // VkImageCreateFlags       flags;
        VK_IMAGE_TYPE_2D,                           // VkImageType              imageType;
        _imageFormat,                               // VkFormat                 format;
        imageSize,                                  // VkExtent3D               extent;
        1u,                                         // uint32_t                 mipLevels;
        1u,                                         // uint32_t                 arrayLayers;
        VK_SAMPLE_COUNT_1_BIT,                      // VkSampleCountFlagBits    samples;
        VK_IMAGE_TILING_OPTIMAL,                    // VkImageTiling            tiling;
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT
            | VK_IMAGE_USAGE_TRANSFER_DST_BIT
            | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,  // VkImageUsageFlags        usage;
        VK_SHARING_MODE_EXCLUSIVE,                  // VkSharingMode            sharingMode;
        1u,                                         // uint32_t                 queueFamilyIndexCount;
        &_queueFamilyIndex,                         // const uint32_t*          pQueueFamilyIndices;
        VK_IMAGE_LAYOUT_UNDEFINED,                  // VkImageLayout            initialLayout;
    };

    vkResult = _vk.vkCreateImage(_device, &imageCreateInfo, _allocator, &_surfaceImage);
    GD_ASSERT(vkResult == VK_SUCCESS && "Creating the surface backing image is failed!");

    VkMemoryRequirements memoryRequirements;
    _vk.vkGetImageMemoryRequirements(_device, _surfaceImage, &memoryRequirements);

    VkMemoryAllocateInfo memoryAllocateInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,                                         // VkStructureType    sType;
        nullptr,                                                                        // const void*        pNext;
        memoryRequirements.size,                                                        // VkDeviceSize       allocationSize;
        getMemoryTypeIndex(memoryRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),    // uint32_t           memoryTypeIndex;
    };

    VkDeviceMemory deviceMemory;
    vkResult = _vk.vkAllocateMemory(_device, &memoryAllocateInfo, _allocator, &deviceMemory);
    GD_ASSERT(vkResult == VK_SUCCESS && "Memory allocation is failed!");

    _memoryAllocations.push_back(deviceMemory);

    vkResult = _vk.vkBindImageMemory(_device, _surfaceImage, deviceMemory, static_cast<VkDeviceSize>(0u));
    GD_ASSERT(vkResult == VK_SUCCESS && "Memory bind is failed!");

    // Clear the surface image

    VkCommandBuffer commandBuffer = _primaryCommandBuffers[0];

    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        nullptr,
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        nullptr,
    };

    VkClearColorValue clearColor = {
        0.0, 0.0, 0.0, 0.0
    };

    VkImageSubresourceRange range = {
        VK_IMAGE_ASPECT_COLOR_BIT,  // VkImageAspectFlags    aspectMask;
        0u,                         // uint32_t              baseMipLevel;
        1u,                         // uint32_t              levelCount;
        0u,                         // uint32_t              baseArrayLayer;
        1u,                         // uint32_t              layerCount;
    };

    _vk.vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

    _vk.vkCmdClearColorImage(commandBuffer, _surfaceImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &range);

    _vk.vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,  // VkStructureType                sType;
        nullptr,                        // const void*                    pNext;
        0,                              // uint32_t                       waitSemaphoreCount;
        nullptr,                        // const VkSemaphore*             pWaitSemaphores;
        nullptr,                        // const VkPipelineStageFlags*    pWaitDstStageMask;
        1,                              // uint32_t                       commandBufferCount;
        &commandBuffer,                 // const VkCommandBuffer*         pCommandBuffers;
        0,                              // uint32_t                       signalSemaphoreCount;
        nullptr,                        // const VkSemaphore*             pSignalSemaphores;
    };

    VkQueue queque;
    _vk.vkGetDeviceQueue(_device, _queueFamilyIndex, 0, &queque);

    VkFenceCreateInfo fenceInfo = {
        VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,    // VkStructureType       sType;
        nullptr,                                // const void*           pNext;
        0,                                      // VkFenceCreateFlags    flags;
    };

    VkFence fence;
    _vk.vkCreateFence(_device, &fenceInfo, _allocator, &fence);

    _vk.vkQueueSubmit(queque, 1, &submitInfo, fence);

    uint64_t timeout = (uint64_t)16 * 1000000; // 16 ms
    _vk.vkWaitForFences(_device, 1, &fence, VK_TRUE, timeout);
    _vk.vkDestroyFence(_device, fence, _allocator);
}

void GepardVulkan::createDefaultFrameBuffer()
{
    VkResult vkResult;

    VkImageViewCreateInfo imageViewCreateInfo = {
        VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,   // VkStructureType            sType;
        nullptr,                                    // const void*                pNext;
        0,                                          // VkImageViewCreateFlags     flags;
        _surfaceImage,                              // VkImage                    image;
        VK_IMAGE_VIEW_TYPE_2D,                      // VkImageViewType            viewType;
        _imageFormat,                               // VkFormat                   format;
        {
            VK_COMPONENT_SWIZZLE_IDENTITY, // swizzle r
            VK_COMPONENT_SWIZZLE_IDENTITY, // swizzle g
            VK_COMPONENT_SWIZZLE_IDENTITY, // swizzle b
            VK_COMPONENT_SWIZZLE_IDENTITY, // swizzle a
        },                                          // VkComponentMapping         components;
        {
            VK_IMAGE_ASPECT_COLOR_BIT,  // VkImageAspectFlags    aspectMask;
            0u,                         // uint32_t              baseMipLevel;
            1u,                         // uint32_t              levelCount;
            0u,                         // uint32_t              baseArrayLayer;
            1u,                         // uint32_t              layerCount;
        },                                          // VkImageSubresourceRange    subresourceRange;
    };

    vkResult = _vk.vkCreateImageView(_device, &imageViewCreateInfo, _allocator, &_frameBufferColorAttachmentImageView);
    GD_ASSERT(vkResult == VK_SUCCESS && "Creating the default frame buffer is failed!");

    std::vector<VkImageView> attachments;
    attachments.push_back(_frameBufferColorAttachmentImageView);

    VkFramebufferCreateInfo frameBufferCreateInfo = {
        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,  // VkStructureType             sType;
        nullptr,                                    // const void*                 pNext;
        0u,                                         // VkFramebufferCreateFlags    flags;
        _renderPass,                                // VkRenderPass                renderPass;
        static_cast<uint32_t>(attachments.size()),  // uint32_t                    attachmentCount;
        attachments.data(),                         // const VkImageView*          pAttachments;
        _surface->width(),                          // uint32_t                    width;
        _surface->height(),                         // uint32_t                    height;
        1u,                                         // uint32_t                    layers;
    };

    vkResult = _vk.vkCreateFramebuffer(_device, &frameBufferCreateInfo, _allocator, &_frameBuffer);
    GD_ASSERT(vkResult == VK_SUCCESS && "Creating the default frame buffer is failed!");
}

uint32_t GepardVulkan::getMemoryTypeIndex(VkMemoryRequirements memoryRequirements, VkMemoryPropertyFlags properties)
{
    /* Algorithm copied from Vulkan specification */
    for (uint32_t i = 0; i < _physicalDeviceMemoryProperties.memoryTypeCount; ++i)
    {
        if ((memoryRequirements.memoryTypeBits & (1 << i)) &&
            ((_physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties))
            return i;
    }

    GD_ASSERT(false && "No feasible memory type index!");
    return -1;
}

void GepardVulkan::logImage(VkImage image, VkExtent3D imageSize)
{
    VkBuffer buffer;
    VkDeviceMemory bufferAlloc;
    VkCommandBuffer commandBuffer = _primaryCommandBuffers[0];
    VkDeviceSize dataSize = imageSize.width * imageSize.height * 4; // assume r8g8b8a8 format

    // Create destination buffer
    VkBufferCreateInfo bufferInfo = {
        VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        nullptr,
        0u,
        dataSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_SHARING_MODE_EXCLUSIVE,
        1u,
        &_queueFamilyIndex,
    };

    _vk.vkCreateBuffer(_device, &bufferInfo, _allocator, &buffer);

    VkMemoryRequirements memoryRequirements;
    _vk.vkGetBufferMemoryRequirements(_device, buffer, &memoryRequirements);
    VkMemoryAllocateInfo allocationInfo = {
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,                                         // VkStructureType    sType;
        nullptr,                                                                        // const void*        pNext;
        memoryRequirements.size,                                                        // VkDeviceSize       allocationSize;
        getMemoryTypeIndex(memoryRequirements, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT),    // uint32_t           memoryTypeIndex;
    };

    _vk.vkAllocateMemory(_device, &allocationInfo, _allocator, &bufferAlloc);
    _vk.vkBindBufferMemory(_device, buffer, bufferAlloc, 0);

    VkImageMemoryBarrier imageBarrier = {
        VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        nullptr,
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        VK_ACCESS_TRANSFER_READ_BIT,
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        VK_QUEUE_FAMILY_IGNORED,
        VK_QUEUE_FAMILY_IGNORED,
        image,
        {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0u,
            1u,
            0u,
            1u
        }
    };

    VkBufferMemoryBarrier bufferBarrier = {
        VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        nullptr,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_HOST_READ_BIT,
        VK_QUEUE_FAMILY_IGNORED,
        VK_QUEUE_FAMILY_IGNORED,
        buffer,
        0u,
        dataSize,
    };

    VkBufferImageCopy copyRegion = {
        0u,
        imageSize.width,
        imageSize.height,
        {
            VK_IMAGE_ASPECT_COLOR_BIT,
            0u,
            0u,
            1u,
        },
        { 0, 0, 0 },
        imageSize,
    };

    VkCommandBufferBeginInfo commandBufferBeginInfo = {
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        nullptr,
        VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        nullptr,
    };

    _vk.vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

    _vk.vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, (VkDependencyFlags)0, 0, (const VkMemoryBarrier*)nullptr, 0, (const VkBufferMemoryBarrier*)nullptr, 1, &imageBarrier);
    _vk.vkCmdCopyImageToBuffer(commandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, buffer, 1, &copyRegion);
    _vk.vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, (VkDependencyFlags)0, 0, (const VkMemoryBarrier*)nullptr, 1, &bufferBarrier, 0, (const VkImageMemoryBarrier*)nullptr);
    _vk.vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {
        VK_STRUCTURE_TYPE_SUBMIT_INFO,  // VkStructureType                sType;
        nullptr,                        // const void*                    pNext;
        0,                              // uint32_t                       waitSemaphoreCount;
        nullptr,                        // const VkSemaphore*             pWaitSemaphores;
        nullptr,                        // const VkPipelineStageFlags*    pWaitDstStageMask;
        1,                              // uint32_t                       commandBufferCount;
        &commandBuffer,                 // const VkCommandBuffer*         pCommandBuffers;
        0,                              // uint32_t                       signalSemaphoreCount;
        nullptr,                        // const VkSemaphore*             pSignalSemaphores;
    };

    VkQueue queque;
    _vk.vkGetDeviceQueue(_device, _queueFamilyIndex, 0, &queque);

    VkFenceCreateInfo fenceInfo = {
        VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,    // VkStructureType       sType;
        nullptr,                                // const void*           pNext;
        0,                                      // VkFenceCreateFlags    flags;
    };

    VkFence fence;
    _vk.vkCreateFence(_device, &fenceInfo, _allocator, &fence);

    _vk.vkQueueSubmit(queque, 1, &submitInfo, fence);

    uint64_t timeout = (uint64_t)16 * 1000000; // 16 ms
    _vk.vkWaitForFences(_device, 1, &fence, VK_TRUE, timeout);

    void* data;
    std::vector<uint8_t> pixelData;
    pixelData.resize(dataSize);
    _vk.vkMapMemory(_device, bufferAlloc, 0, dataSize, 0, &data);

    VkMappedMemoryRange range = {
        VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,  // VkStructureType    sType;
        nullptr,                                // const void*        pNext;
        bufferAlloc,                            // VkDeviceMemory     memory;
        0,                                      // VkDeviceSize       offset;
        dataSize,       // VkDeviceSize       size;
    };

    _vk.vkInvalidateMappedMemoryRanges(_device, 1, &range);
    std::memcpy(pixelData.data(), data, dataSize);


    {
        static int i = 0;
        std::ostringstream _fileName;
        _fileName << "out" << i++ << ".png";
        std::string _title("title");

        const int formatSize = 4;
        uint32_t* buffer = reinterpret_cast<uint32_t*>(pixelData.data());
        FILE *fp = NULL;    //! \todo use 'std::fstream' instead of 'FILE' if possible
        bool code = true;
        png_structp png_ptr = NULL;
        png_infop info_ptr = NULL;
        png_bytep row = NULL;

        // Open file for writing (binary mode)
        fp = fopen(_fileName.str().c_str(), "wb");
        if (fp == NULL) {
            fprintf(stderr, "Could not open file %s for writing\n", _fileName.str().c_str());     //! \todo use 'std::cout/std::clog' instead of 'fprintf'
            code = false;
            goto finalise;  //! \todo ignore 'goto'
        }

        // Initialize write structure
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (png_ptr == NULL) {
            fprintf(stderr, "Could not allocate write struct\n");   //! \todo use 'std::cout/std::clog' instead of 'fprintf'
            code = false;
            goto finalise;  //! \todo ignore 'goto'
        }

        // Initialize info structure
        info_ptr = png_create_info_struct(png_ptr);
        if (info_ptr == NULL) {
            fprintf(stderr, "Could not allocate info struct\n");    //! \todo use 'std::cout/std::clog' instead of 'fprintf'
            code = false;
            goto finalise;  //! \todo ignore 'goto'
        }

        // Setup Exception handling
        if (setjmp(png_jmpbuf(png_ptr))) {
            fprintf(stderr, "Error during png creation\n");     //! \todo use 'std::cout/std::clog' instead of 'fprintf'
            code = false;
            goto finalise;  //! \todo ignore 'goto'
        }

        png_init_io(png_ptr, fp);

        // Write header (8 bit colour depth)
        png_set_IHDR(png_ptr, info_ptr, _surface->width(), _surface->height(),
                     8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

        // Set title
        if (_title.empty()) {
            png_text title_text;
            title_text.compression = PNG_TEXT_COMPRESSION_NONE;
            title_text.key = png_charp("Title");
            title_text.text = png_charp(_title.c_str());
            png_set_text(png_ptr, info_ptr, &title_text, 1);
        }

        png_write_info(png_ptr, info_ptr);

        row = (png_bytep) malloc(formatSize * _surface->width() * sizeof(png_byte));   //! \todo ignore 'malloc', use member variable: _row.

        // Write image data
        uint32_t x, y;
        for (y = 0; y < _surface->height(); ++y) {
            for (x = 0; x < _surface->width(); ++x) {
                row[formatSize * x + 0] = ((buffer[y * _surface->width() + x] & 0x000000ff) >> 0);
                row[formatSize * x + 1] = ((buffer[y * _surface->width() + x] & 0x0000ff00) >> 8);
                row[formatSize * x + 2] = ((buffer[y * _surface->width() + x] & 0x00ff0000) >> 16);
                row[formatSize * x + 3] = ((buffer[y * _surface->width() + x] & 0xff000000) >> 24);
            }
            png_write_row(png_ptr, row);
        }

        // End write
        png_write_end(png_ptr, NULL);

        finalise:   //! \todo ignore 'goto'
        if (fp != NULL) {
            fclose(fp);
        }
        if (info_ptr != NULL) {
            png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
        }
        if (png_ptr != NULL) {
            png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        }
        if (row != NULL) {
            free(row);
        }
    }

    _vk.vkUnmapMemory(_device, bufferAlloc);

    _vk.vkFreeMemory(_device, bufferAlloc, _allocator);
    _vk.vkDestroyFence(_device, fence, _allocator);
    _vk.vkDestroyBuffer(_device, buffer, _allocator);
}

} // namespace vulkan
} // namespace gepard

#endif // GD_USE_VULKAN
