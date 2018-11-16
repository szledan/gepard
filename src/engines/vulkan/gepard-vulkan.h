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

#ifndef GEPARD_VULKAN_H
#define GEPARD_VULKAN_H

#include "gepard-context.h"
#include "gepard-float.h"
#include "gepard-image.h"
#include "gepard-vulkan-interface.h"
#include "gepard.h"

#include <vector>

namespace gepard {

class Image;
class Surface;

namespace vulkan {

class GepardVulkan {
public:
    explicit GepardVulkan(GepardContext&);
    ~GepardVulkan();

    void fillRect(const Float x, const Float y, const Float w, const Float h);
    void drawImage(Image& imagedata, Float sx, Float sy, Float sw, Float sh, Float dx, Float dy, Float dw, Float dh);
    void putImage(Image& imagedata, Float dx, Float dy, Float dirtyX, Float dirtyY, Float dirtyWidth, Float dirtyHeight);
    Image getImage(Float sx, Float sy, Float sw, Float sh);
    void fill();
    void stroke();

private:
    GepardContext& _context;
    GepardVulkanInterface _vk;
    VkAllocationCallbacks* _allocator;
    VkInstance _instance;
    VkPhysicalDevice _physicalDevice;
    VkDevice _device;
    uint32_t _queueFamilyIndex;
    VkCommandPool _commandPool;
    std::vector<VkCommandBuffer> _primaryCommandBuffers;
    std::vector<VkCommandBuffer> _secondaryCommandBuffers;
    std::vector<VkDeviceMemory> _memoryAllocations;
    VkRenderPass _renderPass;
    VkFormat _imageFormat;
    VkImage _surfaceImage;
    VkImageView _frameBufferColorAttachmentImageView;
    VkFramebuffer _frameBuffer;
    VkPhysicalDeviceMemoryProperties _physicalDeviceMemoryProperties;
    VkPhysicalDeviceFeatures _physicalDeviceFeatures;
    VkSurfaceKHR _wsiSurface;
    VkSwapchainKHR _wsiSwapChain;
    std::vector<VkImage> _wsiSwapChainImages;
#ifdef VK_USE_PLATFORM_XCB_KHR
    xcb_connection_t* _xcbConnection;
#endif

    void createDefaultInstance();
    void chooseDefaultPhysicalDevice();
    void chooseDefaultDevice();
    bool chooseGraphicsQueue(const std::vector<VkPhysicalDevice> &devices);
    void createCommandPool();
    void allocatePrimaryCommandBuffer();
    void createDefaultRenderPass();
    void createSurfaceImage();
    void createDefaultFrameBuffer();
    uint32_t getMemoryTypeIndex(const VkMemoryRequirements memoryRequirements, const VkMemoryPropertyFlags properties);
    void createSwapChain();
    void presentImage();
    void presentToMemoryBuffer();
    void readImage(uint32_t* memoryBuffer, int32_t x, int32_t y, uint32_t width, uint32_t height);

    void createBuffer(VkBuffer& buffer, VkDeviceMemory& bufferAlloc, VkMemoryRequirements& bufferRequirements, VkDeviceSize size, VkBufferUsageFlags usageFlag);
    void createImage(VkImage& image, VkDeviceMemory& imageAlloc, VkMemoryRequirements& memReq, VkExtent3D size, VkImageUsageFlags usageFlag);
    void createImageView(VkImageView& imageView, VkImage image);
    void createShaderModule(VkShaderModule& shader, const uint32_t* code, const size_t codeSize);

    void uploadToDeviceMemory(VkDeviceMemory buffer, const void* data, VkDeviceSize size, VkDeviceSize offset = 0);
    void createSimplePipeline(VkPipeline& pipeline,
                              VkPipelineLayout& layout,
                              const VkShaderModule vertex,
                              const VkShaderModule fragment,
                              const VkPipelineVertexInputStateCreateInfo vertexInputState,
                              const VkPipelineColorBlendAttachmentState blendState,
                              const VkPipelineLayoutCreateInfo layoutInfo);

    VkRect2D getDefaultRenderArea();
    VkViewport getDefaultViewPort();

    void submitAndWait(const VkCommandBuffer commandBuffer);
    void updateSurface();
};

// TODO: create a header for these constants
namespace blendMode {
    const VkPipelineColorBlendAttachmentState oneMinusSrcAlpha = {
        VK_TRUE,                                                                                                    // VkBool32                 blendEnable;
        VK_BLEND_FACTOR_SRC_ALPHA,                                                                                  // VkBlendFactor            srcColorBlendFactor;
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,                                                                        // VkBlendFactor            dstColorBlendFactor;
        VK_BLEND_OP_ADD,                                                                                            // VkBlendOp                colorBlendOp;
        VK_BLEND_FACTOR_SRC_ALPHA,                                                                                  // VkBlendFactor            srcAlphaBlendFactor;
        VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,                                                                        // VkBlendFactor            dstAlphaBlendFactor;
        VK_BLEND_OP_ADD,                                                                                            // VkBlendOp                alphaBlendOp;
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT   // VkColorComponentFlags    colorWriteMask;
    };
} // namespace blendMode

} // namespace vulkan

typedef vulkan::GepardVulkan GepardEngineBackend;

} // namespace gepard

#endif // GEPARD_VULKAN_H
