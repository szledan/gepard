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

#ifdef USE_VULKAN

#ifndef GEPARD_VULKAN_INTERFACE_H
#define GEPARD_VULKAN_INTERFACE_H

#include "gepard-defs.h"

#include <vulkan/vulkan.h>

namespace gepard {
namespace vulkan {

class GepardVulkanInterface {
public:
    GepardVulkanInterface(const char* libraryName);
    ~GepardVulkanInterface();

    void loadGlobalFunctions();
    void loadInstanceFunctions(VkInstance instance);
    void loadDeviceFunctions(VkDevice device);

#define GD_VK_DECLARE_FUNCTION(fun) PFN_##fun fun

    // Global level vulkan functions
    GD_VK_DECLARE_FUNCTION(vkGetInstanceProcAddr);
    GD_VK_DECLARE_FUNCTION(vkCreateInstance);

    // Instance level vulkan functions
    GD_VK_DECLARE_FUNCTION(vkDestroyInstance);
    GD_VK_DECLARE_FUNCTION(vkEnumeratePhysicalDevices);
    GD_VK_DECLARE_FUNCTION(vkGetPhysicalDeviceProperties);
    GD_VK_DECLARE_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);

    // Device level vulkan functions

#undef GD_VK_DECLARE_FUNCTION

private:
    void* _vulkanLibrary;
};

} // namespace vulkan
} // namespace gepard

#endif // GEPARD_VULKAN_INTERFACE_H

#endif // USE_VULKAN
