/* Copyright (C) 2016-2017, Gepard Graphics
 * Copyright (C) 2016-2017, Szilard Ledan <szledan@gmail.com>
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

#include "gepard-engine.h"

#include "gepard-color.h"
#include "gepard-defs.h"
#include "gepard-float-point.h"
#include "gepard-float.h"
#include "gepard-logging.h"
#include "gepard-transform.h"

namespace gepard {

/*!
 * \brief GepardEngine::save
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::save()
{
    _context.states.push_back(state());
}

/*!
 * \brief GepardEngine::restore
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::restore()
{
    if (_context.states.size() > 1) {
        _context.states.pop_back();
    }
}

/*!
 * \brief GepardEngine::closePath
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::closePath()
{
    _context.path.pathData()->addCloseSubpathElement();
}

/*!
 * \brief GepardEngine::moveTo
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::moveTo(const Float x, const Float y)
{
    _context.path.pathData()->addMoveToElement(FloatPoint(x, y));
}

/*!
 * \brief GepardEngine::lineTo
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::lineTo(const Float x, const Float y)
{
    _context.path.pathData()->addLineToElement(FloatPoint(x, y));
}

/*!
 * \brief GepardEngine::quadraticCurveTo
 * \param cpx  X-axis value of _control_ poin
 * \param cpy  Y-axis value of _control_ poin
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::quadraticCurveTo(const Float cpx, const Float cpy, const Float x, const Float y)
{
    _context.path.pathData()->addQuadaraticCurveToElement(FloatPoint(cpx, cpy), FloatPoint(x, y));
}

/*!
 * \brief GepardEngine::bezierCurveTo
 * \param cp1x  X-axis value of _first control_ point
 * \param cp1y  Y-axis value of _first control_ point
 * \param cp2x  X-axis value of _second control_ point
 * \param cp2y  Y-axis value of _second control_ point
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::bezierCurveTo(const Float cp1x, const Float cp1y, const Float cp2x, const Float cp2y, const Float x, const Float y)
{
    _context.path.pathData()->addBezierCurveToElement(FloatPoint(cp1x, cp1y), FloatPoint(cp2x, cp2y), FloatPoint(x, y));
}

/*!
 * \brief GepardEngine::arcTo
 * \param x1  X-axis value of _tangent_ point
 * \param y1  Y-axis value of _tangent_ point
 * \param x2  X-axis value of _end_ point
 * \param y2  Y-axis value of _end_ point
 * \param radius  size of arc
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::arcTo(const Float x1, const Float y1, const Float x2, const Float y2, const Float radius)
{
    _context.path.pathData()->addArcToElement(FloatPoint(x1, y1), FloatPoint(x2, y2), radius);
}

/*!
 * \brief GepardEngine::rect
 * \param x  X-axis value of _start_ and _end_ point
 * \param y  Y-axis value of _start_ and _end_ point
 * \param w  size on X-axis
 * \param h  size on Y-axis
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::rect(const Float x, const Float y, const Float w, const Float h)
{
    moveTo(x, y);
    lineTo(x + w, y);
    lineTo(x + w, y + h);
    lineTo(x, y + h);
    closePath();
}

/*!
 * \brief GepardEngine::arc
 * \param x  X-axis value of _center_ point
 * \param y  Y-axis value of _center_ point
 * \param radius  size of arc
 * \param startAngle  specify the _start_ point on arc
 * \param endAngle  specify the _end_ point on arc
 * \param counterclockwise  specify the draw direction on arc
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::arc(const Float x, const Float y, const Float radius, const Float startAngle, const Float endAngle, const bool counterclockwise)
{
    _context.path.pathData()->addArcElement(FloatPoint(x, y), FloatPoint(radius, radius), startAngle, endAngle, counterclockwise);
}

void GepardEngine::scale(const Float x, const Float y)
{
    state().transform.scale(x, y);
    _context.path.pathData()->applyTransform(Transform(1, 0, 0, 1, 0, 0).scale(x, y).inverse());
}

void GepardEngine::rotate(const Float angle)
{
    state().transform.rotate(angle);
    _context.path.pathData()->applyTransform(Transform(1, 0, 0, 1, 0, 0).rotate(angle).inverse());
}

void GepardEngine::translate(const Float x, const Float y)
{
    state().transform.translate(x, y);
    _context.path.pathData()->applyTransform(Transform(1, 0, 0, 1, 0, 0).translate(x, y).inverse());
}

void GepardEngine::transform(const Float a, const Float b, const Float c, const Float d, const Float e, const Float f)
{
    Transform t = Transform(a, b, c, d, e, f);
    state().transform.multiply(t);
    _context.path.pathData()->applyTransform(t.inverse());
}

void GepardEngine::setTransform(const Float a, const Float b, const Float c, const Float d, const Float e, const Float f)
{
    state().transform = Transform(a, b, c, d, e, f);
    _context.path.pathData()->applyTransform(state().transform.inverse());
}

/*!
 * \brief GepardEngine::beginPath
 *
 * More information:
 * <a href="https://www.w3.org/TR/2dcontext/#drawing-state">https://www.w3.org/TR/2dcontext/#drawing-state</a>.
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::beginPath()
{
    GD_LOG(DEBUG) << "Clear path data.";
    _context.path.clear();
}

/*!
 * \brief GepardEngine::fill
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::fill()
{
    GD_ASSERT(_engineBackend);
#ifdef GD_USE_GLES2
    _engineBackend->fillPath(context().path.pathData(), state());
#else // !GD_USE_GLES2
    _engineBackend->fill();
#endif // GD_USE_GLES2
}

/*!
 * \brief GepardEngine::stroke
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::stroke()
{
    GD_ASSERT(_engineBackend);
#ifdef GD_USE_GLES2
    _engineBackend->strokePath();
#else // !GD_USE_GLES2
    _engineBackend->stroke();
#endif // GD_USE_GLES2
}

/*!
 * \brief GepardEngine::drawFocusIfNeeded
 *
 * \todo unimplemented function
 */
void GepardEngine::drawFocusIfNeeded(/*Element element*/)
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardEngine::clip
 *
 * \todo unimplemented function
 */
void GepardEngine::clip()
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardEngine::isPointInPath
 * \param x  X-axis value of the given point
 * \param y  Y-axis value of the given point
 * \return  true if the given _point_ is in the current path
 *
 * \todo unimplemented function
 */
bool GepardEngine::isPointInPath(const Float x, const Float y)
{
    GD_NOT_IMPLEMENTED();
    return false;
}

/*!
 * \brief GepardEngine::fillRect
 * \param x  X-axis value of _start_ and _end_ point
 * \param y  Y-axis value of _start_ and _end_ point
 * \param w  size on X-axis
 * \param h  size on Y-axis
 *
 * \todo documentation is missing
 */
void GepardEngine::fillRect(const Float x, const Float y, const Float w, const Float h)
{
    GD_ASSERT(_engineBackend);
#ifdef GD_USE_GLES2
    _engineBackend->fillRect(x, y, w, h, state().fillColor);
#else // !GD_USE_GLES2
    _engineBackend->fillRect(x, y, w, h);
#endif // GD_USE_GLES2
}

/*!
 * \brief GepardEngine::putImage
 * \param imagedata
 * \param dx Horizontal position on the _destination_
 * \param dy Vertical position on the _destination_
 * \param dirtyX Horizontal position on the _source_
 * \param dirtyY Vertical position on the _source_
 * \param dirtyWidth width of the rectangle to be painted
 * \param dirtyHeight height of the rectangle to be painted.
 *
 * Copy the imageData into the canvas.
 */
void GepardEngine::putImage(Image& imagedata, Float dx, Float dy, Float dirtyX, Float dirtyY, Float dirtyWidth, Float dirtyHeight)
{
    GD_ASSERT(_engineBackend);
#ifdef GD_USE_VULKAN
    _engineBackend->putImage(imagedata, dx, dy, dirtyX, dirtyY, dirtyWidth, dirtyHeight);
#else // !GD_USE_VULKAN
    GD_NOT_IMPLEMENTED();
#endif // GD_USE_VULKAN
}

void GepardEngine::drawImage(Image& imagedata, Float sx, Float sy, Float sw, Float sh, Float dx, Float dy, Float dw, Float dh)
{
    GD_ASSERT(_engineBackend);
#ifdef GD_USE_VULKAN
    _engineBackend->drawImage(imagedata, sx, sy, sw, sh, dx, dy, dw, dh);
#else // !GD_USE_VULKAN
    GD_NOT_IMPLEMENTED();
#endif // GD_USE_VULKAN
}

/*!
 * \brief GepardEngine::getImage
 * \param sx Horizontal position on the _canvas_
 * \param sy Vertical position on the _canvas_
 * \param sw width of the rectangle to be copied
 * \param sh height of the rectangle to be copied
 * \return imageData
 *
 * Read back the canvas' data on the given rectangle.
 */
Image GepardEngine::getImage(Float sx, Float sy, Float sw, Float sh)
{
    GD_ASSERT(_engineBackend);
#ifdef GD_USE_VULKAN
    return _engineBackend->getImage(sx, sy, sw, sh);
#else // !GD_USE_VULKAN
    GD_NOT_IMPLEMENTED();
    return Image();
#endif // GD_USE_VULKAN
}

void GepardEngine::setFillColor(const Color& color)
{
    GD_LOG(DEBUG) << "Set fill color (" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")";
    state().fillColor = color;
}

void GepardEngine::setFillColor(const Float red, const Float green, const Float blue, const Float alpha)
{
    setFillColor(Color(red, green, blue, alpha));
}

void GepardEngine::setStrokeColor(const Color& color)
{
    GD_LOG(DEBUG) << "Set stroke color (" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")";
    state().strokeColor = color;
}

GepardState&GepardEngine::state()
{
    return _context.currentState();
}

void GepardEngine::setFillStyle(const std::string& color)
{
    state().fillColor = Color(color);
}

void GepardEngine::setStrokeStyle(const std::string& color)
{
    state().strokeColor = Color(color);
}

void GepardEngine::setLineWidth(const std::string& width)
{
    state().lineWitdh = strToFloat(width);
}

void GepardEngine::setLineCap(const std::string& capMode)
{
    state().lineCapMode = strToLineCap(capMode);
}

void GepardEngine::setLineJoin(const std::string& joinMode)
{
    state().lineJoinMode = strToLineJoin(joinMode);;
}

void GepardEngine::setMiterLimit(const std::string& limit)
{
    state().miterLimit = strToFloat(limit);
}

} // namespace gepard
