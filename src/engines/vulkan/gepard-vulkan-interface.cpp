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

#include "gepard-vulkan-interface.h"

#include <dlfcn.h>

namespace gepard {
namespace vulkan {

GepardVulkanInterface::GepardVulkanInterface(const char *libraryName)
{
    _vulkanLibrary = dlopen(libraryName, RTLD_LAZY);
}

GepardVulkanInterface::~GepardVulkanInterface()
{
    GD_ASSERT(_vulkanLibrary);
    dlclose(_vulkanLibrary);
}

void GepardVulkanInterface::loadGlobalFunctions()
{
#define GD_VK_LOAD_FUNCTION(fun)\
    fun = (PFN_##fun) vkGetInstanceProcAddr (0, #fun); \
    GD_ASSERT(fun && "Couldn't load " #fun "!");

    if (!_vulkanLibrary)
        GD_CRASH("Loading the Vulkan library was unsuccessful!\n");

    vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)dlsym(_vulkanLibrary, "vkGetInstanceProcAddr");
    GD_ASSERT(vkGetInstanceProcAddr && "Couldn't load the vkGetInstanceProcAddr function!");

    GD_VK_GLOBAL_FUNTION_LIST(GD_VK_LOAD_FUNCTION)

#undef GD_VK_LOAD_FUNCTION
}

void GepardVulkanInterface::loadInstanceFunctions(const VkInstance instance)
{
#define GD_VK_LOAD_FUNCTION(fun)\
    fun = (PFN_##fun) vkGetInstanceProcAddr (instance, #fun); \
    GD_ASSERT(fun && "Couldn't load " #fun "!");

    GD_VK_INSTANCE_FUNTION_LIST(GD_VK_LOAD_FUNCTION)

#if defined(VK_USE_PLATFORM_XLIB_KHR)
    GD_VK_LOAD_FUNCTION(vkCreateXlibSurfaceKHR);
#endif // VK_USE_PLATFORM_XLIB_KHR
#if defined(VK_USE_PLATFORM_XCB_KHR)
    GD_VK_LOAD_FUNCTION(vkCreateXcbSurfaceKHR);
#endif // VK_USE_PLATFORM_XCB_KHR

#undef GD_VK_LOAD_FUNCTION
}

void GepardVulkanInterface::loadDeviceFunctions(const VkDevice device)
{
#define GD_VK_LOAD_FUNCTION(fun)\
    fun = (PFN_##fun) vkGetDeviceProcAddr (device, #fun); \
    GD_ASSERT(fun && "Couldn't load " #fun "!");

    GD_VK_DEVICE_FUNTION_LIST(GD_VK_LOAD_FUNCTION)

#undef GD_VK_LOAD_FUNCTION
}

} // namespace vulkan
} // namespace gepard
