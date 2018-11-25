/* Copyright (C) 2015-2018, Gepard Graphics
 * Copyright (C) 2016, 2018 Kristof Kosztyo <kkristof@inf.u-szeged.hu>
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

#ifndef GEPARD_IMAGE_H
#define GEPARD_IMAGE_H

#include <cstdint>
#include <string>
#include <vector>

namespace gepard {

class Image {
public:
    Image();
    Image(uint32_t width, uint32_t height);
    Image(uint32_t width, uint32_t height, const std::vector<uint32_t> &data);
    virtual ~Image();

    const uint32_t width() const;
    const uint32_t height() const;
    const std::vector<uint32_t> &data() const;

private:
    uint32_t _width;
    uint32_t _height;
    std::vector<uint32_t> _data;
};

namespace utils {
    bool savePng(const Image &image, const std::string &fileName);
    Image loadPng(const std::string &fileName);
}

} // namespace gepard

#endif // GEPARD_IMAGE_H
