/* Copyright (C) 2016, Gepard Graphics
 * Copyright (C) 2016, Szilard Ledan <szledan@gmail.com>
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

#ifndef GEPARD_PNG_SURFACE_H
#define GEPARD_PNG_SURFACE_H

#include "gepard.h"

#include <malloc.h>
#include <png.h>
#include <string>

namespace gepard {

/*!
 * \brief A simple PNGSurface class for _Gepard_
 *
 * \todo: documentation is missing.
 */
class PNGSurface : public Surface {
public:
    PNGSurface(uint32_t width = 0, uint32_t height = 0, std::string title = "Rendered with Gepard")
        : Surface(width, height)
        , _buffer(malloc(width * height * sizeof(uint32_t)))
        , _title(title)
    {
    }
    ~PNGSurface()
    {
        if (_buffer) {
            free(_buffer);
        }
    }

    virtual void* getDisplay() { return nullptr; }
    virtual unsigned long getWindow() { return 0; }
    virtual void* getBuffer() { return _buffer; }

    /*!
     * \todo doc is missing
     *
     * This 'save()' based on:
     *  // LibPNG example
     *  // A.Greensted
     *  // http://www.labbookpages.co.uk
     * Link: http://www.labbookpages.co.uk/software/imgProc/libPNG.html
     */
    const bool save(std::string fileName = "gepard.png")
    {
        const int formatSize = 4;
        uint32_t* buffer = (uint32_t*)_buffer;
        FILE *fp = NULL;    //! \todo use 'std::fstream' instead of 'FILE' if possible
        bool code = true;
        png_structp png_ptr = NULL;
        png_infop info_ptr = NULL;
        png_bytep row = NULL;

        // Open file for writing (binary mode)
        fp = fopen(fileName.c_str(), "wb");
        if (fp == NULL) {
            fprintf(stderr, "Could not open file %s for writing\n", fileName.c_str());     //! \todo use 'std::cout/std::clog' instead of 'fprintf'
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
        png_set_IHDR(png_ptr, info_ptr, width(), height(),
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

        row = (png_bytep) malloc(formatSize * width() * sizeof(png_byte));   //! \todo ignore 'malloc', use member variable: _row.

        // Write image data
        uint32_t x, y;
        for (y = 0; y < height(); ++y) {
            for (x = 0; x < width(); ++x) {
                row[formatSize * x + 0] = ((buffer[y * width() + x] & 0x000000ff) >> 0);
                row[formatSize * x + 1] = ((buffer[y * width() + x] & 0x0000ff00) >> 8);
                row[formatSize * x + 2] = ((buffer[y * width() + x] & 0x00ff0000) >> 16);
                row[formatSize * x + 3] = ((buffer[y * width() + x] & 0xff000000) >> 24);
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

        return code;
    }

private:
    void* _buffer;

    std::string _title;
};

} // namespace gepard

#endif // GEPARD_PNG_SURFACE_H
