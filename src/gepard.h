/* Copyright (C) 2015-2016, Gepard Graphics
 * Copyright (C) 2015, Szilard Ledan <szledan@gmail.com>
 * Copyright (C) 2015-2016, Kristof Kosztyo <kkristof@inf.u-szeged.hu>
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
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */

#ifndef GEPARD_H
#define GEPARD_H

#include "gepard-defs.h"

#include "gepard-image.h"
#include "gepard-surface.h"

namespace gepard {

class Image;
class Surface;

class Gepard {
public:
    explicit Gepard(Surface* surface)
        : _surface(surface)
    {}

    /**
     * 2. State (W3-2DContext-2015)
     */
    void save();
    void restore();

    /**
     * 5 Building paths (W3-2DContext-2015)
     */
    void cloeePath();
    void moveTo(float x, float y);
    void lineTo(float x, float y);
    void quadraticCurveTo(float cpx, float cpy, float x, float y);
    void bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y);
    void arcTo(float x1, float y1, float x2, float y2, float radius);
    void rect(float x, float y, float w, float h);
    void arc(float x, float y, float radius, float startAngle, float endAngle, bool counterclockwise = false);

    /**
     * 6. Transformations (W3-2DContext-2015)
     * (default: transform is the identity matrix)
     */
    void scale(float x, float y);
    void rotate(float angle);
    void translate(float x, float y);
    void transform(float a, float b, float c, float d, float e, float f);
    void setTransform(float a, float b, float c, float d, float e, float f);

    /**
     * 9. Rectangles (W3-2DContext-2015)
     */
    void clearRect(float x, float y, float w, float h);
    void fillRect(float x, float y, float w, float h);
    void strokeRect(float x, float y, float w, float h);

    /**
     * 11. Drawing paths to the canvas (W3-2DContext-2015)
     */
    void beginPath();
    void fill();
    void stroke();
    void drawFocusIfNeeded(/*Element element*/);
    void clip();
    bool isPointInPath(float x, float y);

    /**
     * 12. Drawing images (W3-2DContext-2015)
     */
    void drawImage(Image image, float dx, float dy);
    void drawImage(Image image, float dx, float dy, float dw, float dh);
    void drawImage(Image image, float sx, float sy, float sw, float sh,
        float dx, float dy, float dw, float dh);

    /**
     * 14. Pixel manipulation (W3-2DContext-2015)
     */
    Image createImageData(float sw, float sh);
    Image createImageData(Image imagedata);
    Image getImageData(double sx, double sy, double sw, double sh);
    void putImageData(Image imagedata, double dx, double dy);
    void putImageData(Image imagedata, double dx, double dy, double dirtyX, double dirtyY,
        double dirtyWidth, double dirtyHeight);

private:
    Surface* _surface;
};

} // namespace gepard

#endif // GEPARD_H
