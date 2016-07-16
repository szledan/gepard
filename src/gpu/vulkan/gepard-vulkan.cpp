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

#include "gepard-vulkan.h"

#include <vector>

namespace gepard {
namespace vulkan {

GepardVulkan::GepardVulkan(Surface* surface)
    : _surface(surface)
    , _vk("libvulkan.so")
    , _allocator(nullptr)
    , _instance(0)
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
}

GepardVulkan::~GepardVulkan()
{
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
}

void GepardVulkan::closePath()
{
    GD_LOG_FUNC(" ()");
}

void GepardVulkan::createDefaultInstance()
{
    ASSERT(!_instance);

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
    ASSERT(deviceCount);
    GD_LOG3("Physical devices found: " << deviceCount);
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    ASSERT(deviceCount && "Couldn't find any device!");

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

    VkResult vkResult;
    const float queuePriorities[] = { 1.0f };

    VkDeviceQueueCreateInfo deviceQueueCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // VkStructureType             sType;
        nullptr,                                    // const void*                 pNext;
        0u,                                         // VkDeviceQueueCreateFlags    flags;
        _queueFamilyIndex,                          // uint32_t                    queueFamilyIndex;
        1,                                          // uint32_t                    queueCount;
        queuePriorities                             // const float*                pQueuePriorities;
    };

    VkDeviceCreateInfo deviceCreateInfo = {
        VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,   // VkStructureType                  sType;
        nullptr,                                // const void*                      pNext;
        0u,                                     // VkDeviceCreateFlags              flags;
        1,                                      // uint32_t                         queueCreateInfoCount;
        &deviceQueueCreateInfo,                 // const VkDeviceQueueCreateInfo*   pQueueCreateInfos;
        0u,                                     // uint32_t                         enabledLayerCount;
        nullptr,                                // const char* const*               ppEnabledLayerNames;
        0u,                                     // uint32_t                         enabledExtensionCount;
        nullptr,                                // const char* const*               ppEnabledExtensionNames;
        nullptr,                                // const VkPhysicalDeviceFeatures*  pEnabledFeatures;
    };

    vkResult = _vk.vkCreateDevice(_physicalDevice, &deviceCreateInfo, _allocator, &_device);
    ASSERT(vkResult == VK_SUCCESS && "Logical device creation is failed!");
}


void GepardVulkan::createCommandPool()
{
    struct VkCommandPoolCreateInfo commandPoolCreateInfo = {
        VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,         // VkStructureType sType;
        nullptr,                                            // const void* pNext;
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,    // VkCommandPoolCreateFlags flags;
        _queueFamilyIndex,                                  // uint32_t queueFamilyIndex;
    };

    VkResult vkResult;
    vkResult = _vk.vkCreateCommandPool(_device, &commandPoolCreateInfo, _allocator, &_commandPool);

    ASSERT(vkResult == VK_SUCCESS && "Command pool creation is failed!");
}


} // namespace vulkan
} // namespace gepard

#endif // USE_VULKAN
