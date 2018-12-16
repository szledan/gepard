/* Copyright (C) 2015-2017, Gepard Graphics
 * Copyright (C) 2015-2017, Szilard Ledan <szledan@gmail.com>
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
 */

#ifndef GEPARD_H
#define GEPARD_H

#include "gepard-image.h"
#include <string>

namespace gepard {

class GepardEngine;
class Surface;

class Gepard {
    struct Attribute {
    public:
        Attribute(const char* chs = "", void(*cbf)(GepardEngine*, const std::string&) = nullptr, GepardEngine* eng = nullptr);
        template<typename T>
        Attribute(const T value, void(*cbf)(GepardEngine*, const std::string&) = nullptr, GepardEngine* eng = nullptr) : callBackFunction(cbf), engine(eng), data(std::to_string(value)) {}

        Attribute& operator=(const Attribute&);
        Attribute& operator=(const std::string&);
        Attribute& operator=(const char* chs);
        template<typename T>
        Attribute& operator=(const T value) { this->operator=(std::to_string(value)); return *this; }

        operator std::string() const;
        operator double() const;
        operator float() const;
        operator int() const;

        void callFunction();
        void setCallBack(GepardEngine*, void(*func)(GepardEngine*, const std::string&));
        void(*callBackFunction)(GepardEngine*, const std::string&) = nullptr;
        GepardEngine* engine = nullptr;

        std::string data = "";
    };

public:
    explicit Gepard(Surface* surface);
    ~Gepard();

    /*! \name 2. CanvasAPI State
     *
     * \cond
     * \todo Missing short description
     * \endcond
     */
    /// \{

    void save();
    void restore();
    /// \} 2. CanvasAPI State

    /*! \name 5. CanvasAPI Building paths
     *
     *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
     * A path has a list of zero or more subpaths. Each subpath consists of a
     * list of one or more points, connected by straight or curved lines, and a
     * flag indicating whether the subpath is closed or not. A closed subpath
     * is one where the last point of the subpath is connected to the first
     * point of the subpath by a straight line. Subpaths with fewer than two
     * points are ignored when painting the path.
     *  -- <a href="https://www.w3.org/TR/2dcontext/#building-paths">[W3C-2DContext]</a>
     *   </blockquote>
     *
     * \cond
     * \note The documentation contains quotes from the
     * <a href="https://www.w3.org/TR/2dcontext">W3C-2DContext</a>
     * recommandation.  These are closed in a \c \<blockquote\>.
     * \endcond
     */
    /// \{

    /*! \name 3. Line styles
     */
    /// \{

    /*!
     * \brief lineWidth
     *
     *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
     * Returns the current line width.
     *
     * Can be set, to change the line width. Values that are not finite values
     * greater than zero are ignored.
     *  -- <a href="https://www.w3.org/TR/2dcontext/#line-styles">[W3C-2DContext]</a>
     *   </blockquote>
     *
     * \cond
     * \note The documentation contains quotes from the
     * <a href="https://www.w3.org/TR/2dcontext">W3C-2DContext</a>
     * recommandation.  These are closed in a \c \<blockquote\>.
     * \endcond
     */
    Attribute lineWidth = 1.0;
    /*!
     * \brief lineCap
     *
     *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
     * Returns the current line cap style.
     *
     * Can be set, to change the line cap style.
     *
     * The possible line cap styles are "butt", "round", and "square". Other values
     * are ignored.
     *  -- <a href="https://www.w3.org/TR/2dcontext/#line-styles">[W3C-2DContext]</a>
     *   </blockquote>
     *
     * \cond
     * \note The documentation contains quotes from the
     * <a href="https://www.w3.org/TR/2dcontext">W3C-2DContext</a>
     * recommandation.  These are closed in a \c \<blockquote\>.
     * \endcond
     */
    Attribute lineCap = "butt";
    /*!
     * \brief lineJoin
     *
     *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
     * Returns the current line join style.
     *
     * Can be set, to change the line join style.
     *
     * The possible line join styles are "bevel", "round", and "miter". Other values
     * are ignored.
     *  -- <a href="https://www.w3.org/TR/2dcontext/#line-styles">[W3C-2DContext]</a>
     *   </blockquote>
     *
     * \cond
     * \note The documentation contains quotes from the
     * <a href="https://www.w3.org/TR/2dcontext">W3C-2DContext</a>
     * recommandation.  These are closed in a \c \<blockquote\>.
     * \endcond
     */
    Attribute lineJoin = "miter";
    /*!
     * \brief miterLimit
     *
     *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
     * Returns the current miter limit ratio.
     *
     * Can be set, to change the miter limit ratio. Values that are not finite values
     * greater than zero are ignored.
     *  -- <a href="https://www.w3.org/TR/2dcontext/#line-styles">[W3C-2DContext]</a>
     *   </blockquote>
     *
     * \cond
     * \note The documentation contains quotes from the
     * <a href="https://www.w3.org/TR/2dcontext">W3C-2DContext</a>
     * recommandation.  These are closed in a \c \<blockquote\>.
     * \endcond
     */
    Attribute miterLimit = 10;
    /// \}  3. Line styles

    /*!
     * \brief Marks the current subpath as closed, and starts a new subpath
     * with a point the same as the start and end of the newly closed subpath.
     * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-closepath">[W3C-2DContext]</a>
     */
    void closePath();
    /*!
     * \brief Creates a new subpath with the given point.
     *  -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-moveto">[W3C-2DContext]</a>
     * \param x  X-axis value of _end_ point
     * \param y  Y-axis value of _end_ point
     *
     */
    void moveTo(double x, double y);
    /*!
     * \brief Adds the given point to the current subpath, connected to the
     * previous one by a straight line.
     *  -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-lineto">[W3C-2DContext]</a>
     * \param x X-axis value of _end_ point
     * \param y Y-axis value of _end_ point
     */
    void lineTo(double x, double y);
    /*!
     * \brief Adds the given point to the current subpath, connected to the
     * previous one by a quadratic Bézier curve with the given control point.
     *  -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-quadraticcurveto">[W3C-2DContext]</a>
     * \param cpx  X-axis value of _control_ point
     * \param cpy  Y-axis value of _control_ point
     * \param x  X-axis value of _end_ point
     * \param y  Y-axis value of _end_ point
     */
    void quadraticCurveTo(double cpx, double cpy, double x, double y);
    /*!
     * \brief Adds the given point to the current subpath, connected to the
     * previous one by a cubic Bézier curve with the given control points.
     *  -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-beziercurveto">[W3C-2DContext]</a>
     * \param cp1x  X-axis value of _first control_ point
     * \param cp1y  Y-axis value of _first control_ point
     * \param cp2x  X-axis value of _second control_ point
     * \param cp2y  Y-axis value of _second control_ point
     * \param x  X-axis value of _end_ point
     * \param y  Y-axis value of _end_ point
     */
    void bezierCurveTo(double cp1x, double cp1y, double cp2x, double cp2y, double x, double y);
    /*!
     * \brief Adds an arc with the given control points and radius to the
     * current subpath, connected to the previous point by a straight line.
     *  -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-arcto">[W3C-2DContext]</a>
     * \param x1  X-axis value of _tangent_ point
     * \param y1  Y-axis value of _tangent_ point
     * \param x2  X-axis value of _end_ point
     * \param y2  Y-axis value of _end_ point
     * \param radius  size of arc
     */
    void arcTo(double x1, double y1, double x2, double y2, double radius);
    /*!
     * \brief Adds a new closed subpath to the path, representing the given
     * rectangle.
     *  -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-rect">[W3C-2DContext]</a>
     * \param x  X-axis value of _start_ and _end_ point
     * \param y  Y-axis value of _start_ and _end_ point
     * \param w  size on X-axis
     * \param h  size on Y-axis
     */
    void rect(double x, double y, double w, double h);
    /*!
     * \brief Adds points to the subpath such that the arc described by the
     * circumference of the circle described by the arguments, starting at the
     * given start angle and ending at the given end angle, going in the given
     * direction (defaulting to clockwise), is added to the path, connected to
     * the previous point by a straight line.
     *  -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-arc">[W3C-2DContext]</a>
     * \param x  X-axis value of _center_ point
     * \param y  Y-axis value of _center_ point
     * \param radius  size of arc
     * \param startAngle  specify the _start_ point on arc
     * \param endAngle  specify the _end_ point on arc
     * \param counterclockwise  specify the draw direction on arc
     */
    void arc(double x, double y, double radius, double startAngle, double endAngle, bool counterclockwise = false);
    /// \}  5. CanvasAPI Building paths

    /*! \name 6. CanvasAPI Transformations
     *
     * \cond
     * \todo Missing short description
     * \endcond
     */
    /// \{

    void scale(double x, double y);
    void rotate(double angle);
    void translate(double x, double y);
    void transform(double a, double b, double c, double d, double e, double f);
    void setTransform(double a, double b, double c, double d, double e, double f);
    /// \} 6. CanvasAPI Transformations

    /*! \name 8. Fill and stroke styles
     *
     * \cond
     * \todo Missing short description
     * \endcond
     */
    /// \{

    /*!
     * \brief fillStyle
     *
     *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
     * Returns the current style used for filling shapes.
     *
     * Can be set, to change the fill style.
     *
     * The style can be either a string containing a CSS color, or a CanvasGradient
     * or CanvasPattern object. Invalid values are ignored.
     *  -- <a href="https://www.w3.org/TR/2dcontext/#fill-and-stroke-styles">[W3C-2DContext]</a>
     *   </blockquote>
     *
     * \cond
     * \note The documentation contains quotes from the
     * <a href="https://www.w3.org/TR/2dcontext">W3C-2DContext</a>
     * recommandation.  These are closed in a \c \<blockquote\>.
     * \endcond
     */
    Attribute fillStyle = "black";
    /*!
     * \brief strokeStyle
     *
     *   <blockquote cite="https://www.w3.org/TR/2dcontext/">
     * Returns the current style used for stroking shapes.
     *
     * Can be set, to change the stroke style.
     *
     * The style can be either a string containing a CSS color, or a CanvasGradient
     * or CanvasPattern object. Invalid values are ignored.
     *  -- <a href="https://www.w3.org/TR/2dcontext/#fill-and-stroke-styles">[W3C-2DContext]</a>
     *   </blockquote>
     *
     * \cond
     * \note The documentation contains quotes from the
     * <a href="https://www.w3.org/TR/2dcontext">W3C-2DContext</a>
     * recommandation.  These are closed in a \c \<blockquote\>.
     * \endcond
     */
    Attribute strokeStyle = "black";
    /// \}  8. Fill and stroke styles

    /*! \name 9. CanvasAPI Rectangles
     *
     * \cond
     * \todo Missing short description
     * \endcond
     */
    /// \{

    void clearRect(double x, double y, double w, double h);
    /*!
     * \brief Paints the given rectangle onto the canvas, using the current fill style.
     *  -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-fillrect">[W3C-2DContext]</a>
     * \param x  coordinates of the left of the rectangle
     * \param y  coordinates of the top of the rectangle
     * \param w  width of the rectangle
     * \param h  height of the rectangle
     */
    void fillRect(double x, double y, double w, double h);
    void strokeRect(double x, double y, double w, double h);
    /// \} 9. CanvasAPI Rectangles

    /*! \name 11. CanvasAPI Drawing paths to the canvas
     *
     * \cond
     * \todo Missing short description
     * \endcond
     */
    /// \{

    /*!
     * \brief Resets the current path.
     * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-beginpath">[W3C-2DContext]</a>
     */
    void beginPath();
    /*!
     * \brief Fills the subpaths of the current path or the given path with the
     * current fill style.
     * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-fill">[W3C-2DContext]</a>
     */
    void fill();
    /*!
     * \brief Strokes the subpaths of the current path or the given path with
     * the current stroke style.
     * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-stroke">[W3C-2DContext]</a>
     */
    void stroke();
    /*!
     * \brief Informs the user of the canvas location for the fallback element,
     * based on the current path. If the given element has focus, draws a focus
     * outline around the current path following the platform or user agent
     * conventions for focus outlines as defined by the user agent.
     * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-drawfocusifneeded">[W3C-2DContext]</a>
     */
    void drawFocusIfNeeded(/*Element element*/);
    /*!
     * \brief Further constrains the clipping region to the current path.
     * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-clip">[W3C-2DContext]</a>
     */
    void clip();
    /*!
     * \brief Returns true if the given point is in the current path.
     * -- <a href="https://www.w3.org/TR/2dcontext/#dom-context-2d-ispointinpath">[W3C-2DContext]</a>
     * \param x  X-axis value of the given point
     * \param y  Y-axis value of the given point
     * \return  true if the given _point_ is in the current path
     */
    bool isPointInPath(double x, double y);
    /// \} 11. CanvasAPI Drawing paths to the canvas

    /*! \name 12. CanvasAPI Drawing images
     *
     * \cond
     * \todo Missing short description
     * \endcond
     */
    /// \{
    void drawImage(Image& image, double dx, double dy);
    void drawImage(Image& image, double dx, double dy, double dw, double dh);
    void drawImage(Image& image, double sx, double sy, double sw, double sh,
        double dx, double dy, double dw, double dh);
    /// \} 12. CanvasAPI Drawing images

    /*! \name 14. CanvasAPI Pixel manipulation
     *
     * \cond
     * \todo Missing short description
     * \endcond
     */
    /// \{

    Image createImageData(double sw, double sh);
    Image createImageData(Image& imagedata);
    const Image getImageData(const double sx, const double sy, const double sw, const double sh);
    void putImageData(Image& imagedata, double dx, double dy);
    void putImageData(Image& imagedata, double dx, double dy, double dirtyX, double dirtyY,
        double dirtyWidth, double dirtyHeight);
    /// \} 14. CanvasAPI Pixel manipulation

    /*! \name A. NonCanvasAPI Functions
     *
     * \cond
     * \todo Missing short description
     * \endcond
     */
    /// \{

    /*!
     * \brief Set fill color with red, green, blue and alpha components
     * \param red  the red channel in range [0, 255]
     * \param green  the green channel in range [0, 255]
     * \param blue  the blue channel in range [0, 255]
     * \param alpha  the alpha channel in range [0.0, 1.0]
     */
    void setFillColor(const int red, const int green, const int blue, const float alpha = 1.0f);
    /*!
     * \brief Set stroke color with red, green, blue and alpha components
     * \param red  the red channel in range [0, 255]
     * \param green  the green channel in range [0, 255]
     * \param blue  the blue channel in range [0, 255]
     * \param alpha  the alpha channel in range [0.0, 1.0]
     */
    void setStrokeColor(const int red, const int green, const int blue, const float alpha = 1.0f);

    /// \} A. NonCanvasAPI Functions

private:
    GepardEngine* _engine;
};

/*!
 * \brief The basic Surface class for _Gepard_
 *
 * \todo: documentation is missing.
 */
class Surface {
public:
    Surface(uint32_t width = 0, uint32_t height = 0)
        : _width(width)
        , _height(height)
    {
    }

    virtual ~Surface();
    virtual void* getDisplay() { return nullptr; }
    virtual unsigned long getWindow() { return -1; }
    virtual void* getBuffer() { return nullptr; }

    const uint32_t width() const { return _width; }
    const uint32_t height() const { return _height; }

protected:
    uint32_t _width;
    uint32_t _height;
};

} // namespace gepard

#endif // GEPARD_H
