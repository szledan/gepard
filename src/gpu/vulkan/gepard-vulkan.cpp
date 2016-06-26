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
    , _vkInstance(0)
{
    GD_LOG1("GepardVulkan");
    _vk.loadGlobalFunctions();
    GD_LOG2(" - Global functions are loaded");
    createDefaultInstance();
    _vk.loadInstanceFunctions(_vkInstance);
    GD_LOG2(" - Instance functions are loaded");
    chooseDefaultDevice();
}

GepardVulkan::~GepardVulkan()
{
    if (_vkInstance) {
        _vk.vkDestroyInstance(_vkInstance, nullptr);
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
    ASSERT(!_vkInstance);

    // todo: find better default arguments
    const VkInstanceCreateInfo instanceCreateInfo = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        nullptr,
        0u,
        nullptr,
        0u,
        nullptr,
        0u,
        nullptr,
    };

    _vk.vkCreateInstance(&instanceCreateInfo, nullptr, &_vkInstance);
}

void GepardVulkan::chooseDefaultPhysicalDevice(VkPhysicalDevice *physicalDevice)
{
    uint32_t deviceCount;
    _vk.vkEnumeratePhysicalDevices(_vkInstance, &deviceCount, nullptr);
    ASSERT(deviceCount);
    GD_LOG2("Physical devices found: " << deviceCount);
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    ASSERT(deviceCount && "Couldn't find any device!");

    std::vector<VkPhysicalDevice> integratedDevices;
    std::vector<VkPhysicalDevice> discreteDevices;
    std::vector<VkPhysicalDevice> otherDevices;
    _vk.vkEnumeratePhysicalDevices(_vkInstance, &deviceCount, physicalDevices.data());
    for (int i = 0; i < deviceCount; i++) {
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

    for (int i = 0; i < discreteDevices.size(); i++) {
        uint32_t queueCount;
        _vk.vkGetPhysicalDeviceQueueFamilyProperties(discreteDevices[i], &queueCount, nullptr);
        GD_LOG3("queueCount: " << queueCount);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueCount);
        _vk.vkGetPhysicalDeviceQueueFamilyProperties(discreteDevices[i], &queueCount, queueFamilyProperties.data());
        for (int j = 0; j < queueCount; j++) {
            if (queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                *physicalDevice = discreteDevices[i];
                return;
            }
        }
    }

    for (int i = 0; i < integratedDevices.size(); i++) {
        uint32_t queueCount;
        _vk.vkGetPhysicalDeviceQueueFamilyProperties(integratedDevices[i], &queueCount, nullptr);
        GD_LOG3("queueCount: " << queueCount);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueCount);
        _vk.vkGetPhysicalDeviceQueueFamilyProperties(integratedDevices[i], &queueCount, queueFamilyProperties.data());
        for (int j = 0; j < queueCount; j++) {
            if (queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                *physicalDevice = integratedDevices[i];
                return;
            }
        }
    }

    for (int i = 0; i < otherDevices.size(); i++) {
        uint32_t queueCount;
        _vk.vkGetPhysicalDeviceQueueFamilyProperties(otherDevices[i], &queueCount, nullptr);
        GD_LOG3("queueCount: " << queueCount);
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueCount);
        _vk.vkGetPhysicalDeviceQueueFamilyProperties(otherDevices[i], &queueCount, queueFamilyProperties.data());
        for (int j = 0; j < queueCount; j++) {
            if (queueFamilyProperties[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                *physicalDevice = otherDevices[i];
                return;
            }
        }
    }

    GD_CRASH("Couldn't find any feasible vulkan device!");
}

void GepardVulkan::chooseDefaultDevice()
{
    VkPhysicalDevice physicalDevice;
    chooseDefaultPhysicalDevice(&physicalDevice);
}



} // namespace vulkan
} // namespace gepard

#endif // USE_VULKAN
