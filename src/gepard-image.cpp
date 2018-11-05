/* Copyright (C) 2018, Gepard Graphics
 * Copyright (C) 2018, Kristof Kosztyo <kkristof@inf.u-szeged.hu>
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

#include "gepard-image.h"

#include "gepard-logging.h"
#include <cstring>
#include <png.h>

namespace gepard {

Image::Image()
    : _width(0)
    , _height(0)
{
}

Image::Image(uint32_t width, uint32_t height)
    : _width(width)
    , _height(height)
{
    _data.resize(width * height);
}

Image::Image(uint32_t width, uint32_t height, const std::vector<uint32_t> &data)
    : _width(width)
    , _height(height)
    , _data(data)
{
}

Image::~Image()
{
}

const uint32_t Image::width() const
{
    return _width;
}

const uint32_t Image::height() const
{
    return _height;
}

const std::vector<uint32_t> &Image::data() const
{
    return _data;
}

bool utils::savePng(const Image &image, const std::string &fileName)
{
    if (image.data().size() == 0u) {
        GD_LOG(WARNING) << "The image is empty!";
        return false;
    }
    png_image pngImage;
    std::memset(&pngImage, 0, sizeof(pngImage));
    pngImage.version = PNG_IMAGE_VERSION;
    pngImage.opaque = nullptr;
    pngImage.width = image.width();
    pngImage.height = image.height();
    pngImage.format = PNG_FORMAT_RGBA;
    pngImage.flags = 0u;
    pngImage.colormap_entries = 0u;
    pngImage.warning_or_error = 0u;

    png_image_write_to_file(&pngImage, fileName.c_str(), 0, image.data().data(), 0, nullptr);
    if(pngImage.warning_or_error) {
        GD_LOG(ERROR) << "Saving the PNG has failed! Error code: " << pngImage.warning_or_error
                << " Error message: '" << pngImage.message << "'";
    }
    return !pngImage.warning_or_error;
}

Image utils::loadPng(const std::string &fileName)
{
    png_image pngImage;
    std::memset(&pngImage, 0, sizeof(pngImage));
    pngImage.version = PNG_IMAGE_VERSION;
    pngImage.opaque = nullptr;
    png_image_begin_read_from_file(&pngImage, fileName.c_str());

    std::vector<uint32_t> imageData;
    if(!pngImage.warning_or_error) {
        imageData.resize(pngImage.width * pngImage.height);

        pngImage.format = PNG_FORMAT_RGBA;
        png_image_finish_read(&pngImage, nullptr, imageData.data(), 0, nullptr);
    }

    if (pngImage.warning_or_error) {
        GD_LOG(ERROR) << "Image (" << fileName << ") loading has failed! Error code: "
                << pngImage.warning_or_error << " Error message: '" << pngImage.message << "'";
        return Image(0u, 0u);
    }
    Image image(pngImage.width, pngImage.height, imageData);
    png_image_free(&pngImage);
    return image;
}

} // namespace gepard
