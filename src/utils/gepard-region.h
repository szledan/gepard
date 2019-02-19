/* Copyright (C) 2015-2016, 2018, Gepard Graphics
 * Copyright (C) 2015, 2018, Szilard Ledan <szledan@gmail.com>
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

#ifndef GEPARD_REGION_H
#define GEPARD_REGION_H

namespace gepard {

/*!
 * \brief GD_REGION_BLOCK_SIZE
 * GD_REGION_BLOCK_SIZE = 2048 bytes minus size of a pointer
 *
 * \internal
 */
#define GD_REGION_BLOCK_SIZE (2048 - (int)sizeof(void*))

/*!
 * \brief The Region class
 * \tparam BLOCK_SIZE  defines the region size; the default size is the
 * GD_REGION_BLOCK_SIZE = 2048 bytes minus size of a pointer.
 *
 * This is a simple class for memory allocation.  It doesn't have free() or
 * realloc(), only alloc() for allocation.  It's used to allocate lots of
 * regions with variating sizes, but which are usually small, and are kept
 * until the whole Region is freed.
 *
 * The Region model determines free space in blocks (which is less than 2 KiB
 * by default).
 *
 * \internal
 */
template<const uint32_t BLOCK_SIZE = GD_REGION_BLOCK_SIZE>
class Region {
public:
    Region()
    {
        _first = new RegionElement();
        _last = _first;
        _fill = 0;
    }
    ~Region()
    {
        while (_first) {
            _last = _first;
            _first = _first->next;
            delete _last;
        }
    }

    /*!
     * \brief Region::alloc
     *
     * \param size  size of required memory in bytes
     * \return  pointer to allocated memory or nullptr if allocation failed.
     *
     * \internal
     */
    void* alloc(uint32_t size)
    {
        if (size <= BLOCK_SIZE) {

            if (_fill + size > BLOCK_SIZE) {
                _last->next = new RegionElement();
                _last = _last->next;
                _fill = 0;
            }

            void* ptr = _last->value + _fill;
            _fill += size;

            return ptr;
        }

        return nullptr;
    }

private:
    struct RegionElement {
        RegionElement() : next(nullptr) {}

        RegionElement* next;
        uint8_t value[BLOCK_SIZE];
    };

    RegionElement* _first = nullptr;
    RegionElement* _last = nullptr;
    uint32_t _fill = 0;
};

} // namespace gepard

#endif // GEPARD_TYPES_H
