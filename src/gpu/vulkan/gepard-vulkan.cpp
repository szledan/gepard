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

namespace gepard {

namespace vulkan {

GepardVulkan::GepardVulkan(Surface* surface)
    : _surface(surface)
    , _vk("libvulkan.so")
    , _vkInstance(0)
{
    LOG1("GepardVulkan");
    _vk.loadGlobalFunctions();
    LOG1(" - Global functions are loaded");
    createDefaultInstance();
    _vk.loadInstanceFunctions(&_vkInstance);
    LOG1(" - Instance functions are loaded");
}

GepardVulkan::~GepardVulkan()
{
    if (_vkInstance) {
        _vk.vkDestroyInstance(_vkInstance, nullptr);
    }
}

void GepardVulkan::fillRect(Float x, Float y, Float w, Float h)
{
    LOG1("fillRect");
}

void GepardVulkan::closePath()
{
    LOG1("closePath");
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

} // namespace vulkan

} // namespace gepard

#endif // USE_VULKAN
