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

#ifndef GEPARD_VULKAN_INTERFACE_H
#define GEPARD_VULKAN_INTERFACE_H

#include "gepard-defs.h"

// TODO: move this define to the build system
#define VK_USE_PLATFORM_XLIB_KHR
//#define VK_USE_PLATFORM_XCB_KHR

#include <vulkan/vulkan.h>

namespace gepard {
namespace vulkan {

// Global level vulkan functions
#define GD_VK_GLOBAL_FUNTION_LIST(FUNC) \
    FUNC(vkCreateInstance); \
    FUNC(vkEnumerateInstanceExtensionProperties);

// Instance level vulkan functions
#define GD_VK_INSTANCE_FUNTION_LIST(FUNC) \
    FUNC(vkDestroyInstance); \
    FUNC(vkEnumeratePhysicalDevices); \
    FUNC(vkGetPhysicalDeviceProperties); \
    FUNC(vkGetPhysicalDeviceQueueFamilyProperties); \
    FUNC(vkCreateDevice); \
    FUNC(vkGetDeviceProcAddr); \
    FUNC(vkGetPhysicalDeviceMemoryProperties); \
    FUNC(vkGetPhysicalDeviceFeatures); \
    FUNC(vkDestroySurfaceKHR); \
    FUNC(vkGetPhysicalDeviceSurfaceFormatsKHR); \
    FUNC(vkGetPhysicalDeviceSurfaceCapabilitiesKHR); \
    FUNC(vkGetPhysicalDeviceSurfacePresentModesKHR);

// Device level vulkan functions
#define GD_VK_DEVICE_FUNTION_LIST(FUNC) \
    FUNC(vkDestroyDevice); \
    FUNC(vkGetDeviceQueue); \
    FUNC(vkCreateCommandPool); \
    FUNC(vkResetCommandPool); \
    FUNC(vkDestroyCommandPool); \
    FUNC(vkAllocateCommandBuffers); \
    FUNC(vkFreeCommandBuffers); \
    FUNC(vkBeginCommandBuffer); \
    FUNC(vkEndCommandBuffer); \
    FUNC(vkQueueSubmit); \
    FUNC(vkCreateRenderPass); \
    FUNC(vkDestroyRenderPass); \
    FUNC(vkCreateFramebuffer); \
    FUNC(vkDestroyFramebuffer); \
    FUNC(vkCreateImage); \
    FUNC(vkDestroyImage); \
    FUNC(vkCreateImageView); \
    FUNC(vkDestroyImageView); \
    FUNC(vkGetBufferMemoryRequirements); \
    FUNC(vkGetImageMemoryRequirements); \
    FUNC(vkBindBufferMemory); \
    FUNC(vkBindImageMemory); \
    FUNC(vkAllocateMemory); \
    FUNC(vkFreeMemory); \
    FUNC(vkCreateGraphicsPipelines); \
    FUNC(vkDestroyPipeline); \
    FUNC(vkCreateShaderModule); \
    FUNC(vkDestroyShaderModule); \
    FUNC(vkCreatePipelineLayout); \
    FUNC(vkDestroyPipelineLayout); \
    FUNC(vkCmdBeginRenderPass); \
    FUNC(vkCmdEndRenderPass); \
    FUNC(vkCreateBuffer); \
    FUNC(vkDestroyBuffer); \
    FUNC(vkMapMemory); \
    FUNC(vkFlushMappedMemoryRanges); \
    FUNC(vkInvalidateMappedMemoryRanges); \
    FUNC(vkUnmapMemory); \
    FUNC(vkCmdBindPipeline); \
    FUNC(vkCmdDraw); \
    FUNC(vkCmdDrawIndexed); \
    FUNC(vkCmdDrawIndirect); \
    FUNC(vkCmdDrawIndexedIndirect); \
    FUNC(vkCreateFence); \
    FUNC(vkDestroyFence); \
    FUNC(vkResetFences); \
    FUNC(vkWaitForFences); \
    FUNC(vkCmdBindVertexBuffers); \
    FUNC(vkCmdBindIndexBuffer); \
    FUNC(vkCmdPipelineBarrier); \
    FUNC(vkCmdCopyBuffer); \
    FUNC(vkCmdCopyBufferToImage); \
    FUNC(vkCmdCopyImageToBuffer); \
    FUNC(vkCmdCopyImage); \
    FUNC(vkCmdBlitImage); \
    FUNC(vkCmdClearColorImage); \
    FUNC(vkCmdClearAttachments); \
    FUNC(vkCreateSwapchainKHR); \
    FUNC(vkDestroySwapchainKHR); \
    FUNC(vkGetSwapchainImagesKHR); \
    FUNC(vkAcquireNextImageKHR); \
    FUNC(vkQueuePresentKHR); \
    FUNC(vkCreateDescriptorSetLayout); \
    FUNC(vkDestroyDescriptorSetLayout); \
    FUNC(vkCreateDescriptorPool); \
    FUNC(vkDestroyDescriptorPool); \
    FUNC(vkAllocateDescriptorSets); \
    FUNC(vkFreeDescriptorSets); \
    FUNC(vkCreateSampler); \
    FUNC(vkDestroySampler); \
    FUNC(vkUpdateDescriptorSets); \
    FUNC(vkCmdBindDescriptorSets);

class GepardVulkanInterface {
public:
    GepardVulkanInterface(const char* libraryName);
    ~GepardVulkanInterface();

    void loadGlobalFunctions();
    void loadInstanceFunctions(const VkInstance instance);
    void loadDeviceFunctions(const VkDevice device);

#define GD_VK_DECLARE_FUNCTION(fun) PFN_##fun fun

    // Global level vulkan functions
    GD_VK_DECLARE_FUNCTION(vkGetInstanceProcAddr);
    GD_VK_GLOBAL_FUNTION_LIST(GD_VK_DECLARE_FUNCTION)

    GD_VK_INSTANCE_FUNTION_LIST(GD_VK_DECLARE_FUNCTION)
#if defined(VK_USE_PLATFORM_XLIB_KHR)
    GD_VK_DECLARE_FUNCTION(vkCreateXlibSurfaceKHR);
#endif // VK_USE_PLATFORM_XLIB_KHR
#if defined(VK_USE_PLATFORM_XCB_KHR)
    GD_VK_DECLARE_FUNCTION(vkCreateXcbSurfaceKHR);
#endif // VK_USE_PLATFORM_XCB_KHR

    GD_VK_DEVICE_FUNTION_LIST(GD_VK_DECLARE_FUNCTION)

#undef GD_VK_DECLARE_FUNCTION

private:
    void* _vulkanLibrary;
};

} // namespace vulkan
} // namespace gepard

#endif // GEPARD_VULKAN_INTERFACE_H

#endif // USE_VULKAN
