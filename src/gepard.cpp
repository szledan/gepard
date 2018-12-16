/* Copyright (C) 2015-2017, Gepard Graphics
 * Copyright (C) 2015-2016, Kristof Kosztyo <kkristof@inf.u-szeged.hu>
 * Copyright (C) 2015-2017, Szilard Ledan <szledan@gmail.com>
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

#include "gepard.h"

#include "gepard-color.h"
#include "gepard-context.h"
#include "gepard-defs.h"
#include "gepard-engine.h"
#include "gepard-image.h"
#include "gepard-logging.h"
#include <map>

namespace gepard {

Gepard::Attribute::Attribute(const char* chs, void (*cbf)(GepardEngine*, const std::string&), GepardEngine* eng)
    : callBackFunction(cbf)
    , engine(eng)
    , data(chs)
{
}

Gepard::Attribute& Gepard::Attribute::operator=(const Attribute& atr)
{
    callBackFunction = atr.callBackFunction;
    engine = atr.engine;
    data = atr.data;
    callFunction();

    return *this;
}

Gepard::Attribute& Gepard::Attribute::operator=(const std::string& str)
{
    data = str;
    callFunction();

    return *this;
}

Gepard::Attribute& Gepard::Attribute::operator=(const char* chs)
{
    this->operator=(std::string(chs)); return *this;
}

gepard::Gepard::Attribute::operator std::string() const
{
    return data;
}

gepard::Gepard::Attribute::operator double() const
{
    return std::stod(data);
}

gepard::Gepard::Attribute::operator float() const
{
    return std::stof(data);
}

gepard::Gepard::Attribute::operator int() const
{
    return std::stoi(data);
}

void Gepard::Attribute::callFunction()
{
    if (engine && callBackFunction) {
        callBackFunction(engine, data);
    }
}

void Gepard::Attribute::setCallBack(GepardEngine* eng, void(*func)(GepardEngine*, const std::string&))
{
    engine = eng;
    callBackFunction = func;
}

Gepard::Gepard(Surface* surface)
    : _engine(new GepardEngine(surface))
{
    fillStyle.setCallBack(_engine, [](GepardEngine* engine, const std::string& color){ engine->setFillStyle(color); });
    strokeStyle.setCallBack(_engine, [](GepardEngine* engine, const std::string& color){ engine->setStrokeStyle(color); });
    lineWidth.setCallBack(_engine, [](GepardEngine* engine, const std::string& width){ engine->setLineWidth(width); });
    lineCap.setCallBack(_engine, [](GepardEngine* engine, const std::string& capMode){ engine->setLineCap(capMode); });
    lineJoin.setCallBack(_engine, [](GepardEngine* engine, const std::string& joinMode){ engine->setLineJoin(joinMode); });
    miterLimit.setCallBack(_engine, [](GepardEngine* engine, const std::string& limit){ engine->setMiterLimit(limit); });
}

Gepard::~Gepard()
{
    if (_engine) {
        delete _engine;
    }
}

/*! \todo missing docs */
void Gepard::save()
{
    _engine->save();
}

/*! \todo missing docs */
void Gepard::restore()
{
    _engine->restore();
}

/*!
 * \brief Gepard::closePath
 *
 *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
 *
 * The closePath() method must do nothing if the object's path has no subpaths.
 * Otherwise, it must mark the last subpath as closed, create a new subpath
 * whose first point is the same as the previous subpath's first point, and
 * finally add this new subpath to the path.
 *
 * \note
 * If the last subpath had more than one point in its list of points,
 * then this is equivalent to adding a straight line connecting the last point
 * back to the first point, thus "closing" the shape, and then repeating the
 * last (possibly implied) moveTo() call.
 *
 *  -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-closepath">[W3C-2DContext]</a>
 *   </blockquote>
 */
void Gepard::closePath()
{
    GD_ASSERT(_engine);
    _engine->closePath();
}

/*!
 * \brief Gepard::moveTo
 *
 *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
 *
 * The moveTo(x, y) method must create a new subpath with the specified point
 * as its first (and only) point.
 *
 * When the user agent is to ensure there is a subpath for a coordinate (x, y)
 * on a path, the user agent must check to see if the path has any subpaths,
 * and if it does not, then the user agent must create a new subpath with the
 * point (x, y) as its first (and only) point, as if the moveTo() method had
 * been called.
 * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-moveto">[W3C-2DContext]</a>
 *   </blockquote>
 */
void Gepard::moveTo(double x, double y)
{
    GD_ASSERT(_engine);
    _engine->moveTo(x, y);
}

/*!
 * \brief Gepard::lineTo
 *
 *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
 *
 * The lineTo(x, y) method must ensure there is a subpath for (x, y) if the
 * object's path has no subpaths. Otherwise, it must connect the last point in
 * the subpath to the given point (x, y) using a straight line, and must then
 * add the given point (x, y) to the subpath.
 *
 * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-lineto">[W3C-2DContext]</a>
 *   </blockquote>
 */
void Gepard::lineTo(double x, double y)
{
    GD_ASSERT(_engine);
    _engine->lineTo(x, y);
}

/*!
 * \brief Gepard::quadraticCurveTo
 *
 *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
 *
 * The quadraticCurveTo(cpx, cpy, x, y) method must ensure there is a subpath
 * for (cpx, cpy), and then must connect the last point in the subpath to the
 * given point (x, y) using a quadratic Bézier curve with control point (cpx,
 * cpy), and must then add the given point (x, y) to the subpath.
 *
 * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-quadraticcurveto">[W3C-2DContext]</a>
 *   </blockquote>
 */
void Gepard::quadraticCurveTo(double cpx, double cpy, double x, double y)
{
    GD_ASSERT(_engine);
    _engine->quadraticCurveTo(cpx, cpy, x, y);
}

/*!
 * \brief Gepard::bezierCurveTo
 *
 *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
 *
 * The bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y) method must ensure there is
 * a subpath for (cp1x, cp1y), and then must connect the last point in the
 * subpath to the given point (x, y) using a cubic Bézier curve with control
 * points (cp1x, cp1y) and (cp2x, cp2y). Then, it must add the point (x, y) to
 * the subpath.
 *
 * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-beziercurveto">[W3C-2DContext]</a>
 *   </blockquote>
 */
void Gepard::bezierCurveTo(double cp1x, double cp1y, double cp2x, double cp2y, double x, double y)
{
    GD_ASSERT(_engine);
    _engine->bezierCurveTo(cp1x, cp1y, cp2x, cp2y, x, y);
}

/*!
 * \brief Gepard::arcTo
 *
 *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
 *
 * The arcTo(x1, y1, x2, y2, radius) method must first ensure there is a
 * subpath for (x1, y1). Then, the behavior depends on the arguments and the
 * last point in the subpath, as described below.
 *
 * Negative values for radius must cause the implementation to throw an
 * IndexSizeError exception.
 *
 * Let the point (x0, y0) be the last point in the subpath.
 *
 * If the point (x0, y0) is equal to the point (x1, y1), or if the point (x1,
 * y1) is equal to the point (x2, y2), or if the radius radius is zero, then
 * the method must add the point (x1, y1) to the subpath, and connect that
 * point to the previous point (x0, y0) by a straight line.
 *
 * Otherwise, if the points (x0, y0), (x1, y1), and (x2, y2) all lie on a
 * single straight line, then the method must add the point (x1, y1) to the
 * subpath, and connect that point to the previous point (x0, y0) by a straight
 * line.
 *
 * Otherwise, let The Arc be the shortest arc given by circumference of the
 * circle that has radius radius, and that has one point tangent to the
 * half-infinite line that crosses the point (x0, y0) and ends at the point
 * (x1, y1), and that has a different point tangent to the half-infinite line
 * that ends at the point (x1, y1), and crosses the point (x2, y2). The points
 * at which this circle touches these two lines are called the start and end
 * tangent points respectively. The method must connect the point (x0, y0) to
 * the start tangent point by a straight line, adding the start tangent point
 * to the subpath, and then must connect the start tangent point to the end
 * tangent point by The Arc, adding the end tangent point to the subpath.
 *
 * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-arcto">[W3C-2DContext]</a>
 *   </blockquote>
 */
void Gepard::arcTo(double x1, double y1, double x2, double y2, double radius)
{
    GD_ASSERT(_engine);
    _engine->arcTo(x1, y1, x2, y2, radius);
}

/*!
 * \brief Gepard::rect
 *
 *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
 *
 * The rect(x, y, w, h) method must create a new subpath containing just the
 * four points (x, y), (x+w, y), (x+w, y+h), (x, y+h), with those four points
 * connected by straight lines, and must then mark the subpath as closed. It
 * must then create a new subpath with the point (x, y) as the only point in
 * the subpath.
 *
 * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-rect">[W3C-2DContext]</a>
 *   </blockquote>
 */
void Gepard::rect(double x, double y, double w, double h)
{
    GD_ASSERT(_engine);
    _engine->rect(x, y, w, h);
}

/*!
 * \brief Gepard::arc
 *
 *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
 *
 * The arc(x, y, radius, startAngle, endAngle, counterclockwise) method draws
 * an arc.
 *
 * If the context has any subpaths, then the method must add a straight line
 * from the last point in the subpath to the start point of the arc. In any
 * case, it must draw the arc between the start point of the arc and the end
 * point of the arc, and add the start and end points of the arc to the
 * subpath. The arc and its start and end points are defined as follows:
 *
 * Consider a circle that has its origin at (x, y), and that has radius radius.
 * The points at startAngle and endAngle this circle's circumference, measured
 * in radians clockwise from the positive x-axis, are the start and end points
 * respectively.
 *
 * If the counterclockwise argument false and endAngle-startAngle is equal to
 * or greater than 2\f$\pi\f$, or if the counterclockwise argument is true and
 * startAngle-endAngle is equal to or greater than 2\f$\pi\f$, then the arc is the
 * whole circumference of this circle.
 *
 * Otherwise, the arc is the path along the circumference of this circle from
 * the start point to the end point, going anti-clockwise if the
 * counterclockwise argument is true, and clockwise otherwise. Since the points
 * are on the circle, as opposed to being simply angles from zero, the arc can
 * never cover an angle greater than 2\f$\pi\f$ radians. If the two points are the
 * same, or if the radius is zero, then the arc is defined as being of zero
 * length in both directions.
 *
 * Negative values for radius must cause the implementation to throw an
 * IndexSizeError exception.
 *
 * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-arc">[W3C-2DContext]</a>
 *   </blockquote>
 */
void Gepard::arc(double x, double y, double radius, double startAngle, double endAngle, bool counterclockwise)
{
    GD_ASSERT(_engine);
    _engine->arc(x, y, radius, startAngle, endAngle, counterclockwise);
}

/*! \todo missing docs */
void Gepard::scale(double x, double y)
{
    _engine->scale(x, y);
}

/*! \todo missing docs */
void Gepard::rotate(double angle)
{
    _engine->rotate(angle);
}

/*! \todo missing docs */
void Gepard::translate(double x, double y)
{
    _engine->translate(x, y);
}

/*! \todo missing docs */
void Gepard::transform(double a, double b, double c, double d, double e, double f)
{
    _engine->transform(a, b, c, d, e, f);
}

/*! \todo missing docs */
void Gepard::setTransform(double a, double b, double c, double d, double e, double f)
{
    _engine->setTransform(a, b, c, d, e, f);
}

void Gepard::clearRect(double x, double y, double w, double h)
{
/*! \todo unimplemented function */
}

/*!
 * \brief Gepard::fillRect
 *
 *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
 *
 * The fillRect(x, y, w, h) method must paint the specified rectangular area
 * using the fillStyle. If either height or width are zero, this method has no
 * effect.
 *
 * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-fillrect">[W3C-2DContext]</a>
 *   </blockquote>
 */
void Gepard::fillRect(double x, double y, double w, double h)
{
    GD_ASSERT(_engine);
    _engine->fillRect(x, y, w, h);
}

void Gepard::strokeRect(double x, double y, double w, double h)
{
/*! \todo unimplemented function */
}

/*!
 * \brief Gepard::beginPath
 *
 *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
 *
 * The beginPath() method must empty the list of subpaths in the context's
 * current path so that the it once again has zero subpaths.
 *
 * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-beginpath">[W3C-2DContext]</a>
 *   </blockquote>
 */
void Gepard::beginPath()
{
    GD_ASSERT(_engine);
    _engine->beginPath();
}

/*!
 * \brief Gepard::fill
 *
 *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
 *
 * The fill() method must fill all the subpaths of the current path, using
 * fillStyle, and using the non-zero winding number rule. Open subpaths must
 * be implicitly closed when being filled (without affecting the actual subpaths).
 *
 * \note Thus, if two overlapping but otherwise independent subpaths have
 * opposite windings, they cancel out and result in no fill. If they have the
 * same winding, that area just gets painted once.
 *
 * Paths, when filled or stroked, must be painted without affecting the current
 * path, and must be subject to shadow effects, global alpha, the clipping
 * region, and global composition operators.
 *
 * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-fill">[W3C-2DContext]</a>
 *   </blockquote>
 *
 * \todo unimplemented function
 */
void Gepard::fill()
{
    GD_ASSERT(_engine);
    _engine->fill();
}

/*!
 * \brief Gepard::stroke
 *
 *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
 *
 * The stroke() method must trace the path, using the CanvasRenderingContext2D
 * object for the line styles, and then fill the combined stroke area using the
 * strokeStyle attribute.
 *
 * \note As a result of how the algorithm to trace a path is defined,
 * overlapping parts of the paths in one stroke operation are treated as if
 * their union was what was painted.
 *
 * \note The stroke style is affected by the transformation during painting,
 * even if the path is the current default path.
 *
 * Paths, when filled or stroked, must be painted without affecting the current
 * path, and must be subject to shadow effects, global alpha, the clipping
 * region, and global composition operators.
 *
 * Zero-length line segments must be pruned before stroking a path. Empty
 * subpaths must be ignored.
 *
 * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-stroke">[W3C-2DContext]</a>
 *   </blockquote>
 *
 * \todo unimplemented function
 */
void Gepard::stroke()
{
    GD_ASSERT(_engine);
    _engine->stroke();
}

/*!
 * \brief Gepard::drawFocusIfNeeded
 *
 *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
 *
 * The drawFocusIfNeeded(element) method, when invoked, must run the following steps:
 *
 *   1. If the current path has zero subpaths, then abort these steps.
 *
 *   2. If element is not focused or is not a descendant of the element with whose
 *   context the method is associated, then abort these steps.
 *
 *   3. If the user has requested the use of particular focus outlines (e.g.
 *   high-contrast focus outlines), or if the element would have a focus
 *   outline drawn around it, then draw a focus outline of the appropriate
 *   style along the intended path, following platform conventions.\n
 *   \note Some platforms only draw focus outlines around elements that have
 *   been focused from the keyboard, and not those focused from the mouse.
 *   Other platforms simply don't draw focus outlines around some elements at
 *   all unless relevant accessibility features are enabled.
 *
 *   This API is intended to follow these conventions. User agents that
 *   implement distinctions based on the manner in which the element was
 *   focused are encouraged to classify focus driven by the focus() method
 *   based on the kind of user interaction event from which the call was
 *   triggered (if any).\n
 *   The focus outline should not be subject to the shadow effects, the global
 *   alpha, or the global composition operators, but should be subject to the
 *   clipping region. When the focus area is clipped by the canvas element,
 *   only the visual representation of the focus outline is clipped to the
 *   clipping region. If the focus area is not on the screen, then scroll the
 *   focus outline into view when it receives focus.
 *
 *   4. Inform the user of the location given by the path. The full location of
 *   the corresponding fallback element is passed to the accessibility API, if
 *   supported. User agents may wait until the next time the event loop reaches
 *   its "update the rendering" step to inform the user.
 *
 *   \note "Inform the user", as used in this section, could mean calling a
 *   system accessibility API, which would notify assistive technologies such
 *   as magnification tools. To properly drive magnification based on a focus
 *   change, a system accessibility API driving a screen magnifier needs the
 *   bounds for the newly focused object. The methods above are intended to
 *   enable this by allowing the user agent to report the bounding box of the
 *   path used to render the focus outline as the bounds of the element element
 *   passed as an argument, if that element is focused, and the bounding box of
 *   the area to which the user agent is scrolling as the bounding box of the
 *   current selection.
 *
 * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-drawfocusifneeded">[W3C-2DContext]</a>
 *   </blockquote>
 *
 * \todo unimplemented function
 */
void Gepard::drawFocusIfNeeded(/*Element element*/)
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief Gepard::clip
 *
 *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
 *
 * The clip() method must create a new clipping region by calculating the
 * intersection of the current clipping region and the area described by the
 * path, using the non-zero winding number rule. Open subpaths must be
 * implicitly closed when computing the clipping region, without affecting the
 * actual subpaths. The new clipping region replaces the current clipping
 * region.
 *
 * When the context is initialized, the clipping region must be set to the
 * rectangle with the top left corner at (0,0) and the width and height of the
 * coordinate space.
 *
 * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-clip">[W3C-2DContext]</a>
 *   </blockquote>
 *
 * \todo unimplemented function
 */
void Gepard::clip()
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief Gepard::isPointInPath
 *
 *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
 *
 * The isPointInPath() method must return true if the point given by the x and
 * y coordinates passed to the method, when treated as coordinates in the
 * canvas coordinate space unaffected by the current transformation, is inside
 * the intended path as determined by the non-zero winding number rule; and
 * must return false otherwise. Points on the path itself must be considered to
 * be inside the path. If either of the arguments is infinite or NaN, then the
 * method must return false.
 *
 * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-ispointinpath">[W3C-2DContext]</a>
 *   </blockquote>
 *
 * \todo unimplemented function
 */
bool Gepard::isPointInPath(double x, double y)
{
    GD_NOT_IMPLEMENTED();
    return false;
}

void Gepard::drawImage(Image& image, double dx, double dy)
{
    double width = image.width();
    double height = image.height();
    _engine->drawImage(image, 0.0, 0.0, width, height, dx, dy, width, height);
}

void Gepard::drawImage(Image& image, double dx, double dy, double dw, double dh)
{
    _engine->drawImage(image, 0.0, 0.0, image.width(), image.height(), dx, dy, dw, dh);
}

void Gepard::drawImage(Image& image, double sx, double sy, double sw, double sh,
    double dx, double dy, double dw, double dh)
{
    _engine->drawImage(image, sx, sy, sw, sh, dx, dy, dw, dh);
}

Image Gepard::createImageData(double sw, double sh)
{
    GD_ASSERT(sw >= 0.0 && sh >= 0.0);
    uint32_t width = (uint32_t) sw;
    uint32_t height = (uint32_t) sh;
    return Image(width, height);
}

Image Gepard::createImageData(Image& imagedata)
{
    uint32_t width = imagedata.width();
    uint32_t height = imagedata.height();
    return Image(width, height);
}

const Image Gepard::getImageData(const double sx, const double sy, const double sw, const double sh)
{
    return _engine->getImage(sx, sy, sw, sh);
}

void Gepard::putImageData(Image& imagedata, double dx, double dy)
{
    _engine->putImage(imagedata, dx, dy, 0.0, 0.0, imagedata.width(), imagedata.height());
}

void Gepard::putImageData(Image& imagedata, double dx, double dy, double dirtyX, double dirtyY,
    double dirtyWidth, double dirtyHeight)
{
    _engine->putImage(imagedata, dx, dy, dirtyX, dirtyY, dirtyWidth, dirtyHeight);
}

void Gepard::setFillColor(const int red, const int green, const int blue, const float alpha)
{
    GD_ASSERT(_engine);
    const float ratio = 1.0f / 255.0f;
    _engine->setFillColor(Color(ratio * float(red), ratio * float(green), ratio * float(blue), float(alpha)));
}

void Gepard::setStrokeColor(const int red, const int green, const int blue, const float alpha)
{
    GD_ASSERT(_engine);
    const float ratio = 1.0f / 255.0f;
    _engine->setStrokeColor(Color(ratio * float(red), ratio * float(green), ratio * float(blue), float(alpha)));
}

// Virtual destructor definition for the abstract Surface class.
Surface::~Surface()
{
}

} // namespace gepard
