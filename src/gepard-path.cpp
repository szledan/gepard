/* Copyright (C) 2015, Szilard Ledan <szledan@gmail.com>
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

#include "config.h"
#include "gepard-path.h"

#include <list>
#include <map>
#include <math.h>
#include <memory>

namespace gepard {

/* Fill path shaders */

#define ANTIALIAS_LEVEL 16

const GLchar* fillPathVertexShader = PROGRAM(
    precision highp float;
    attribute vec4 a_position;
    attribute vec4 a_x1x2Cdx1dx2;

    // To reduce the rounding issues of float16 variables,
    // the numbers are spearated for integer and fractional parts.
    // On the downside, this doubles the required arguments.
    varying vec4 v_y1y2;
    varying vec4 v_x1x2;
    varying vec2 v_dx1dx2;

    uniform vec2 u_viewportSize;

    void main(void)
    {
        // a_position[2] = trapezoid.bottomY    < a_position[1]
        // a_position[3] = trapezoid.topY       > a_position[1]
        float y = floor(a_position[2]);
        v_y1y2[0] = a_position[1] - y;
        v_y1y2[1] = floor(a_position[3]) - y;
        v_y1y2[2] = fract(a_position[2]);
        v_y1y2[3] = fract(a_position[3]);

        float x = floor(a_x1x2Cdx1dx2[0]);
        v_x1x2[0] = a_position[0] - x;
        v_x1x2[1] = floor(a_x1x2Cdx1dx2[1]) - x;
        v_x1x2[2] = fract(a_x1x2Cdx1dx2[0]);
        v_x1x2[3] = fract(a_x1x2Cdx1dx2[1]);

        // slopeLeft
        v_dx1dx2[0] = a_x1x2Cdx1dx2[2];
        // slopeRight
        v_dx1dx2[1] = a_x1x2Cdx1dx2[3];

        gl_Position = vec4((2.0 * a_position.xy / u_viewportSize) - 1.0, 0.0, 1.0);
    }
);

const GLchar* fillPathFragmentShader = PROGRAM(
    precision highp float;

    varying vec4 v_y1y2;
    varying vec4 v_x1x2;
    varying vec2 v_dx1dx2;

    void main(void)
    {
        const float step = 1.0 / ANTIALIAS_LEVEL.0;
        const float rounding = 0.5 / ANTIALIAS_LEVEL.0;

        float y = floor(v_y1y2[0]);
        float from = max(-y + v_y1y2[2], 0.0);
        float to = min(v_y1y2[1] - y + v_y1y2[3], 1.0) - from;

        vec2 x1x2 = (y + from) * (v_dx1dx2 * ANTIALIAS_LEVEL.0);

        float x = floor(v_x1x2[0]);
        x1x2[0] = (-x) + (x1x2[0] + v_x1x2[2]);
        x1x2[1] = (v_x1x2[1] - x) + (x1x2[1] + v_x1x2[3]);

        // Alpha value to must be > 0.
        float alpha = 1.0;

        float sum = (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0));
        if (to > 1.0 - rounding) {
            vec2 last = x1x2 + v_dx1dx2 * (ANTIALIAS_LEVEL.0 - 1.0);
            sum += (clamp(last[1], 0.0, 1.0) - clamp(last[0], 0.0, 1.0));
            to -= step;
        }

        if (sum <= 2.0 - rounding) {
            x1x2 += v_dx1dx2;

            while (to >= rounding) {
                sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0));
                x1x2 += v_dx1dx2;
                to -= step;
            }

            alpha = sum * (step);
        }

        gl_FragColor = vec4(0.0, 0.0, 0.0, alpha);
    }
);

/* Copy texture shaders */

const GLchar* copyPathVertexShader = PROGRAM(
    precision highp float;

    attribute vec4 a_position;

    varying vec2 v_texturePosition;

    void main()
    {
        v_texturePosition = a_position.zw;
        gl_Position = vec4((2.0 * a_position.xy / 512.0) - 1.0, 0.0, 1.0);
    }
);

const GLchar* copyPathFragmentShader = PROGRAM(
    precision highp float;

    uniform sampler2D u_texture;
    uniform vec4 u_color;
    varying vec2 v_texturePosition;

    void main()
    {
        gl_FragColor = vec4(u_color.rgb, texture2D(u_texture, v_texturePosition).a);
    }
);

/* Sheder helper functions */

static void compileShaderProg(GLuint* result, const GLchar *vertexShaderSource, const GLchar *fragmentShaderSource)
{
    GLuint shaderProgram = 0;
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    GLint intValue;
    GLchar *log = NULL;
    GLsizei length;

    if (*result)
        return;

    // Shader programs are zero terminated.
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &intValue);
    if (intValue != GL_TRUE) {
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &intValue);
        log = (GLchar*)malloc(intValue);
        glGetShaderInfoLog(vertexShader, intValue, &length, log);
        printf("Vertex shader compilation failed with: %s\n", log);
        goto error;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &intValue);
    if (intValue != GL_TRUE) {
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &intValue);
        log = (GLchar*)malloc(intValue);
        glGetShaderInfoLog(fragmentShader, intValue, &length, log);
        printf("Fragment shader compilation failed with: %s\n", log);
        goto error;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &intValue);
    if (intValue != GL_TRUE) {
        GLchar *log;
        GLsizei length = -13;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &intValue);
        log = (GLchar*)malloc(intValue);
        glGetProgramInfoLog(fragmentShader, intValue, &length, log);
        printf("Shader program link failed with: %s\n", log);
        goto error;
    }

    // According to the specification, the shaders are kept
    // around until the program object is freed (reference counted).
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    *result = shaderProgram;
    return;

error:
    printf("Warning: Shader program cannot be compiled!\n");
    if (log)
        free(log);
    if (vertexShader)
        glDeleteShader(vertexShader);
    if (fragmentShader)
        glDeleteShader(fragmentShader);
    if (shaderProgram)
        glDeleteProgram(shaderProgram);
}

static void setupAttributes(Trapezoid& trapezoid, GLfloat* attributes, int antiAliasingLevel)
{
    GLfloat slopeLeft = (trapezoid.topLeftX - trapezoid.bottomLeftX) / (trapezoid.topY - trapezoid.bottomY);
    GLfloat slopeRight = (trapezoid.topRightX - trapezoid.bottomRightX) / (trapezoid.topY - trapezoid.bottomY);
    const GLfloat bottomY = floor(trapezoid.bottomY);
    const GLfloat topY = ceil(trapezoid.topY);
    // The fraction is stored in a temporary variable.
    GLfloat temp, x1, x2;
    GLfloat bottomLeftX, bottomRightX, topLeftX, topRightX;

    temp = ceil(trapezoid.topY) - (trapezoid.topY);
    x1 = trapezoid.topLeftX - temp * slopeLeft;
    x2 = trapezoid.topRightX - temp * slopeRight;
    topLeftX = floor(x1 - fabs(slopeLeft));
    topRightX = ceil(x2 + fabs(slopeRight));

    temp = trapezoid.bottomY - floor(trapezoid.bottomY);
    x1 = trapezoid.bottomLeftX - temp * slopeLeft;
    x2 = trapezoid.bottomRightX - temp * slopeRight;
    bottomLeftX = floor(x1 - fabs(slopeLeft));
    bottomRightX = ceil(x2 + fabs(slopeRight));

    slopeLeft /= ANTIALIAS_LEVEL;
    slopeRight /= ANTIALIAS_LEVEL;

    ASSERT(topY != bottomY);
    for (int i = 0; i < 4; i++) {
        // Absolute coordinates are transformed to the [-1,+1] space.
        switch (i) {
        case 0:
            *attributes++ = (bottomLeftX);
            *attributes++ = (bottomY);
            break;

        case 1:
            *attributes++ = (topLeftX);
            *attributes++ = (topY);
            break;

        case 2:
            *attributes++ = (bottomRightX);
            *attributes++ = (bottomY);
            break;

        case 3:
            *attributes++ = (topRightX);
            *attributes++ = (topY);
            break;
        }

        *attributes++ = (trapezoid.bottomY);
        *attributes++ = (trapezoid.topY);

        *attributes++ = (x1);
        *attributes++ = (x2);
        *attributes++ = (slopeLeft);
        *attributes++ = (slopeRight);
    }
}

/* Path */

static inline int min(int l, int r) { return r < l ? r : l; }

void Path::fillPath()
{
    ASSERT(_pathData.lastElement()->isCloseSubpath());

    _pathData.dump();

    if (!_surface)
        return;

    // 1. Tessellating the path.
    TrapezoidTessellator tt(this, TrapezoidTessellator::FillRule::NonZero, ANTIALIAS_LEVEL);
    TrapezoidList trapezoidList = tt.trapezoidList();
    const int trapezoidCount = trapezoidList.size();

    // 2. Using OpenGL ES 2.0 spec. to drawing.
    static const int s_kMaximumNumberOfAttributes = 65536;
    static const int s_kMaximumNumberOfUshortQuads = 65536 / 6;

    // 2.a Init and bind quad indexes.
    static GLuint s_indexBufferObject;
    {
        GLushort* quadIndexes;
        const int bufferSize = s_kMaximumNumberOfUshortQuads * 6;

        quadIndexes = reinterpret_cast<GLushort*>(malloc(bufferSize * sizeof(GLushort)));

        GLushort* currentQuad = quadIndexes;
        GLushort* currentQuadEnd = quadIndexes + bufferSize;

        int index = 0;
        while (currentQuad < currentQuadEnd) {
            currentQuad[0] = index;
            currentQuad[1] = index + 1;
            currentQuad[2] = index + 2;
            currentQuad[3] = index + 1;
            currentQuad[4] = index + 2;
            currentQuad[5] = index + 3;
            currentQuad += 6;
            index += 4;
        }

        glGenBuffers(1, &s_indexBufferObject);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_indexBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize * sizeof(GLushort), quadIndexes, GL_STATIC_DRAW);

        free(quadIndexes);
    }

    // 2.b Allocate a buffer for shader attributes.
    GLfloat* attributes = reinterpret_cast<GLfloat*>(malloc(s_kMaximumNumberOfAttributes * sizeof(GLfloat)));

    // 2.c Bind alpha texture for the path.
    GLuint pathTexture;
    GLuint fbo;

    glGenTextures(1, &pathTexture);
    glBindTexture(GL_TEXTURE_2D, pathTexture);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 512.0, 512.0, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pathTexture, 0);

    // 2.d Enable and set blend mode.
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    // 2.e Set clear color.
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // 2.f Compile and use shader programs.
    static GLuint fillPathShader = 0;
    compileShaderProg(&fillPathShader, fillPathVertexShader, fillPathFragmentShader);
    glUseProgram(fillPathShader);

    // 2.g Set attribute buffers.
    const int strideLength = 8 * sizeof(GLfloat);
    GLint intValue;

    intValue = glGetAttribLocation(fillPathShader, "a_position");
    glEnableVertexAttribArray(intValue);
    glVertexAttribPointer(intValue, 4, GL_FLOAT, GL_FALSE, strideLength, attributes);

    intValue = glGetAttribLocation(fillPathShader, "a_x1x2Cdx1dx2");
    glEnableVertexAttribArray(intValue);
    glVertexAttribPointer(intValue, 4, GL_FLOAT, GL_FALSE, strideLength, attributes + 4);

    intValue = glGetUniformLocation(fillPathShader, "u_viewportSize");
    glUniform2f(intValue, 512.0f, 512.0f);

    // 2.h Draw trapezoids.
    std::cout << "Trapezoids (" << trapezoidList.size() << "): ";
    int trapezoidIndex = 0;
    for (Trapezoid trapezoid : trapezoidList) {
//        std::cout << trapezoid << " ";
        ASSERT(trapezoid.bottomY < trapezoid.topY);
        ASSERT(trapezoid.bottomLeftX <= trapezoid.bottomRightX);
        ASSERT(trapezoid.topLeftX <= trapezoid.topRightX);

        setupAttributes(trapezoid, attributes + trapezoidIndex * 32, tt.antiAliasingLevel());
        trapezoidIndex++;
        if (trapezoidIndex >= min(s_kMaximumNumberOfUshortQuads, s_kMaximumNumberOfAttributes / 32)) {
            glDrawElements(GL_TRIANGLES, 6 * trapezoidIndex, GL_UNSIGNED_SHORT, nullptr);
            trapezoidIndex = 0;
        }
    }
    std::cout << std::endl;

    if (trapezoidIndex) {
        glDrawElements(GL_TRIANGLES, 6 * trapezoidCount, GL_UNSIGNED_SHORT, nullptr);
    }

    // 3. Copy alpha texture to the display.
    // 3.a Bind primary (display) framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3.b Set blend mode.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 3.c Set clear color.
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // 3.d Compile and use shader programs.
    static GLuint copyPathShader = 0;
    compileShaderProg(&copyPathShader, copyPathVertexShader, copyPathFragmentShader);

    glUseProgram(copyPathShader);

    // 3.e Set attribute buffers.
    // TODO: use viewport size:
    static GLfloat textureCoords[] = { 0, 512, 0, 1, 0, 0, 0, 0, 512, 512, 1, 1, 512, 0, 1, 0 };

    intValue = glGetAttribLocation(copyPathShader, "a_position");
    glEnableVertexAttribArray(intValue);
    glVertexAttribPointer(intValue, 4, GL_FLOAT, GL_FALSE, 0, textureCoords);

    intValue = glGetUniformLocation(copyPathShader, "u_texture");
    glBindTexture(GL_TEXTURE_2D, pathTexture);

    // 3.f Set color of path.
    intValue = glGetUniformLocation(copyPathShader, "u_color");
    glUniform4f(intValue, 0.0f, 0.4f, 0.7f, 0.0f);

    // 3.g Copy path to display.
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 4. Drawing is ready. Swap buffers.
    eglSwapBuffers(_surface->eglDisplay(), _surface->eglSurface());

    // TODO: Remove this line. (only testing)
    printf("glGetError: %d\n", glGetError());

    free(attributes);
}

void PathData::addMoveToElement(FloatPoint to)
{
    if (_lastElement && _lastElement->isMoveTo()) {
        _lastElement->to = to;
        return;
    }

    PathElement* currentElement = static_cast<PathElement*>(new (_region.alloc(sizeof(MoveToElement))) MoveToElement(to));

    if (!_firstElement) {
        _firstElement = currentElement;
        _lastElement = _firstElement;
    } else {
        _lastElement->next = currentElement;
        _lastElement = _lastElement->next;
    }

    _lastMoveToElement = _lastElement;
}

void PathData::addLineToElement(FloatPoint to)
{
    if (!_lastElement) {
        addMoveToElement(to);
    }

    if (!_lastElement->isMoveTo() && _lastElement->to == to)
        return;

    _lastElement->next = static_cast<PathElement*>(new (_region.alloc(sizeof(LineToElement))) LineToElement(to));
    _lastElement = _lastElement->next;
}

void PathData::addQuadaraticCurveToElement(FloatPoint control, FloatPoint to)
{
    if (!_lastElement) {
        addMoveToElement(to);
    }

    _lastElement->next = static_cast<PathElement*>(new (_region.alloc(sizeof(QuadraticCurveToElement))) QuadraticCurveToElement(control, to));
    _lastElement = _lastElement->next;
}

void PathData::addBezierCurveToElement(FloatPoint control1, FloatPoint control2, FloatPoint to)
{
    if (!_lastElement) {
        addMoveToElement(to);
    }

    _lastElement->next = static_cast<PathElement*>(new (_region.alloc(sizeof(BezierCurveToElement))) BezierCurveToElement(control1, control2, to));
    _lastElement = _lastElement->next;
}

void PathData::addArcElement(FloatPoint center, FloatPoint radius, Float startAngle, Float endAngle, bool antiClockwise)
{
    if (!_lastElement) {
        addMoveToElement(center);
    }

    _lastElement->next = static_cast<PathElement*>(new (_region.alloc(sizeof(ArcElement))) ArcElement(center, radius, startAngle, endAngle, antiClockwise));
    _lastElement = _lastElement->next;
}

void PathData::addCloseSubpath()
{
    if (!_lastElement || _lastElement->isCloseSubpath())
        return;

    if (_lastElement->isMoveTo()) {
        addLineToElement(_lastElement->to);
    }

    _lastElement->next = static_cast<PathElement*>(new (_region.alloc(sizeof(CloseSubpathElement))) CloseSubpathElement(_lastMoveToElement->to));
    _lastElement = _lastElement->next;
}

void PathData::dump()
{
    PathElement* element = _firstElement;

    std::cout << "firstElement: " << _firstElement << std::endl;
    std::cout << "lastElement: " << _lastElement << std::endl;
    std::cout << "lastMoveToElement: " << _lastMoveToElement << std::endl;
    std::cout << "PathData:";
    while (element) {
        std::cout << " " << *element;
        element = element->next;
    }
    std::cout << std::endl;
}

/* SegmentApproximator */

void SegmentApproximator::insertSegment(FloatPoint from, FloatPoint to)
{
    ASSERT(from.y != to.y);

    Segment segment(from, to);

    // Update bounding-box.
    _boundingBox.stretch(segment.from);
    _boundingBox.stretch(segment.to);

    // Insert segment.
    const int topY = segment.topY();
    const int bottomY = segment.bottomY();

    if (_segments.find(topY) == _segments.end()) {
        _segments.emplace(topY, new SegmentList()).first->second->push_front(segment);
        // TODO: log: std::cout << *(_segments[topY].begin()) << std::endl;
    } else {
        _segments[topY]->push_front(segment);
    }

    if (_segments.find(bottomY) == _segments.end()) {
        _segments.emplace(bottomY, new SegmentList());
    }
}

void SegmentApproximator::insertLine(FloatPoint from, FloatPoint to)
{
    from.x *= _kAntiAliasLevel;
    from.y = floor(from.y * _kAntiAliasLevel);
    to.x *= _kAntiAliasLevel;
    to.y = floor(to.y * _kAntiAliasLevel);

    if (from.y == to.y)
        return;

    insertSegment(from, to);
}

static inline Float abs(const Float &t) { return (t < 0) ? -t : t; }

bool SegmentApproximator::quadCurveIsLineSegment(FloatPoint points[])
{
    const Float x0 = points[0].x;
    const Float y0 = points[0].y;
    const Float x1 = points[1].x;
    const Float y1 = points[1].y;
    const Float x2 = points[2].x;
    const Float y2 = points[2].y;

    const Float dt = abs((x2 - x0) * (y0 - y1) - (x0 - x1) * (y2 - y0));

    if (dt > _kTolerance)
        return false;

    Float minX, minY, maxX, maxY;

    if (x0 < x2) {
        minX = x0 - _kTolerance;
        maxX = x2 + _kTolerance;
    } else {
        minX = x2 - _kTolerance;
        maxX = x0 + _kTolerance;
    }

    if (y0 < y2) {
        minY = y0 - _kTolerance;
        maxY = y2 + _kTolerance;
    } else {
        minY = y2 - _kTolerance;
        maxY = y0 + _kTolerance;
    }

    return !(x1 < minX || x1 > maxX || y1 < minY || y1 > maxY);
}

void SegmentApproximator::splitQuadraticCurve(FloatPoint points[])
{
    const FloatPoint a = points[0];
    const FloatPoint b = points[1];
    const FloatPoint c = points[2];
    const FloatPoint ab = (a + b) / 2.0;
    const FloatPoint bc = (b + c) / 2.0;
    const FloatPoint mid = (ab + bc) / 2.0;

    points[0] = a;
    points[1] = ab;
    points[2] = mid;
    points[3] = bc;
    points[4] = c;
}

void SegmentApproximator::insertQuadCurve(const FloatPoint from, const FloatPoint control, const FloatPoint to)
{
    // De Casteljau algorithm.
    const int max = 16 * 2 + 1;
    FloatPoint buffer[max];
    FloatPoint* points = buffer;

    points[0] = from;
    points[1] = control;
    points[2] = to;

    do {
        if (quadCurveIsLineSegment(points)) {
            insertLine(points[0], points[2]);
            points -= 2;
            continue;
        }

        splitQuadraticCurve(points);
        points += 2;

        if (points >= buffer + max - 3) {
            // This recursive code path is rarely executed.
            insertQuadCurve(points[0], points[1], points[2]);
            points -= 2;
        }
    } while (points >= buffer);
}

bool SegmentApproximator::curveIsLineSegment(FloatPoint points[])
{
    const Float x0 = points[0].x;
    const Float y0 = points[0].y;
    const Float x1 = points[1].x;
    const Float y1 = points[1].y;
    const Float x2 = points[2].x;
    const Float y2 = points[2].y;
    const Float x3 = points[3].x;
    const Float y3 = points[3].y;

    const Float dt1 = abs((x3 - x0) * (y0 - y1) - (x0 - x1) * (y3 - y0));
    const Float dt2 = abs((x3 - x0) * (y0 - y2) - (x0 - x2) * (y3 - y0));

    if (dt1 > _kTolerance || dt2 > _kTolerance)
        return false;

    Float minX, minY, maxX, maxY;

    if (x0 < x3) {
        minX = x0 - _kTolerance;
        maxX = x3 + _kTolerance;
    } else {
        minX = x3 - _kTolerance;
        maxX = x0 + _kTolerance;
    }

    if (y0 < y3) {
        minY = y0 - _kTolerance;
        maxY = y3 + _kTolerance;
    } else {
        minY = y3 - _kTolerance;
        maxY = y0 + _kTolerance;
    }

    return !(x1 < minX || x1 > maxX || y1 < minY || y1 > maxY
        || x2 < minX || x2 > maxX || y2 < minY || y2 > maxY);
}

void SegmentApproximator::splitCubeCurve(FloatPoint points[])
{
    const FloatPoint a = points[0];
    const FloatPoint b = points[1];
    const FloatPoint c = points[2];
    const FloatPoint d = points[3];
    const FloatPoint ab = (a + b) / 2.0;
    const FloatPoint bc = (b + c) / 2.0;
    const FloatPoint cd = (c + d) / 2.0;
    const FloatPoint abbc = (ab + bc) / 2.0;
    const FloatPoint bccd = (bc + cd) / 2.0;
    const FloatPoint mid = (abbc + bccd) / 2.0;

    points[0] = a;
    points[1] = ab;
    points[2] = abbc;
    points[3] = mid;
    points[4] = bccd;
    points[5] = cd;
    points[6] = d;
}

void SegmentApproximator::insertBezierCurve(const FloatPoint from, const FloatPoint control1, const FloatPoint control2, const FloatPoint to)
{
    // De Casteljau algorithm.
    const int max = 16 * 3 + 1;
    FloatPoint buffer[max];
    FloatPoint* points = buffer;

    points[0] = from;
    points[1] = control1;
    points[2] = control2;
    points[3] = to;

    do {
        if (curveIsLineSegment(points)) {
            insertLine(points[0], points[3]);
            points -= 3;
            continue;
        }

        splitCubeCurve(points);
        points += 3;

        if (points >= buffer + max - 4) {
            // This recursive code path is rarely executed.
            insertBezierCurve(points[0], points[1], points[2], points[3]);
            points -= 3;
        }
    } while (points >= buffer);
}

void SegmentApproximator::insertArc(const FloatPoint center, const FloatPoint radius, const Float startAngle, const Float endAngle, const bool antiClockwise)
{
    // TODO: Missing approximation!
}

void SegmentApproximator::printSegements()
{
    for (auto& currentSegments : _segments) {
//        std::cout << currentSegments.first << ": ";
        SegmentList currentList = *(currentSegments.second);
        for (Segment& segment : currentList) {
//            std::cout << segment << " ";
            std::cout << segment << std::endl;
        }
//        std::cout << std::endl;
    }
}

inline void SegmentApproximator::splitSegments()
{
    for (SegmentTree::iterator currentSegments = _segments.begin(); currentSegments != _segments.end(); ++currentSegments) {
        SegmentTree::iterator newSegments = currentSegments;
        ++newSegments;
        if (newSegments != _segments.end()) {
            SegmentList* currentList = currentSegments->second;
            SegmentList* newList = newSegments->second;
            int splitY = floor(newSegments->first);
            ASSERT(currentList && newList);

            for (Segment& segment : *currentList) {
                if (segment.isOnSegment(splitY)) {
                    newList->push_front(segment.splitSegment(splitY));
                }
            }
        }
    }
}

SegmentList* SegmentApproximator::segments()
{
    // 1. Split segments with all y lines.
    splitSegments();

    // 2. Find all intersection points.
    for (auto& currentSegments : _segments) {
        SegmentList* currentList = currentSegments.second;
        SegmentList::iterator currentSegment = currentList->begin();
        for (SegmentList::iterator segment = currentSegment; currentSegment != currentList->end(); ++segment) {
            if (segment != currentList->end()) {
                const Float y = currentSegment->computeIntersectionY(&(*segment));
                if (y != NAN && y != INFINITY) {
                    _segments.emplace(floor(y), new SegmentList());
                    if (floor(y) != y)
                        _segments.emplace(floor(y) + 1, new SegmentList());
                }
            } else {
                segment = ++currentSegment;
            }
        }
    }

    // 3. Split segments with all y lines.
    splitSegments();

    // 4. Merge and sort all segment list.
    SegmentList* segments = new SegmentList();
    for (SegmentTree::iterator currentSegments = _segments.begin(); currentSegments != _segments.end(); ++currentSegments) {
        SegmentList* currentList = currentSegments->second;
        currentList->sort();
        segments->merge(*currentList);
    }

    // 5. Fix intersection pairs.
    for (SegmentList::iterator segment = segments->begin(); segment != segments->end(); ++segment) {
        ASSERT(segment->to.y - segment->from.y >= 1);
        for (SegmentList::iterator furtherSegment = segment; (furtherSegment != segments->end()) && (segment->from.y == furtherSegment->from.y) ; ++furtherSegment) {
            ASSERT(segment->to.y == furtherSegment->to.y);
            if (furtherSegment->from.x < segment->from.x) {
                furtherSegment->from.x = segment->from.x;
                ASSERT(furtherSegment->to.x >= segment->to.x);
            }
            if (furtherSegment->to.x < segment->to.x) {
                furtherSegment->to.x = segment->to.x;
            }
        }
    }

    // 6. Return independent segments.
    return segments;
}

/* TrapezoidTessellator */

TrapezoidList TrapezoidTessellator::trapezoidList()
{
    PathElement* element = _path->pathData().firstElement();

    if (!element)
        return TrapezoidList();

    ASSERT(element->type == PathElementTypes::MoveTo);

    const Float halfPixelPrecision = 1.0;
    SegmentApproximator segmentApproximator(_antiAliasingLevel, halfPixelPrecision);
    FloatPoint from;
    FloatPoint to = element->to;
    FloatPoint lastMoveTo = to;

    // 1. Insert path elements.
    do {
        from = to;
        element = element->next;
        to = element->to;
        switch (element->type) {
        case PathElementTypes::MoveTo: {
            segmentApproximator.insertLine(from, lastMoveTo);
            lastMoveTo = to;
            break;
        }
        case PathElementTypes::LineTo: {
            segmentApproximator.insertLine(from, to);
            break;
        }
        case PathElementTypes::CloseSubpath: {
            segmentApproximator.insertLine(from, lastMoveTo);
            lastMoveTo = to;
            break;
        }
        case PathElementTypes::QuadraticCurve: {
            QuadraticCurveToElement* qe = reinterpret_cast<QuadraticCurveToElement*>(element);
            segmentApproximator.insertQuadCurve(from, qe->control, to);
            break;
        }
        case PathElementTypes::BezierCurve: {
            BezierCurveToElement* be = reinterpret_cast<BezierCurveToElement*>(element);
            segmentApproximator.insertBezierCurve(from, be->control1, be->control2, to);
            break;
        }
        case PathElementTypes::Arc: {
            // TODO: Not implemented!
            break;
        }
        case PathElementTypes::Undefined:
        default:
            // unreachable
            break;
        }
    } while (element->next);

    segmentApproximator.insertLine(lastMoveTo, element->to);

    // 2. Use approximator to generate the list of segments.
    SegmentList* segmentList = segmentApproximator.segments();
    TrapezoidList trapezoidList;

    const Float denom = _antiAliasingLevel * 1 + 0;
    if (segmentList) {
        // 3. Generate trapezoids.
        Trapezoid trapezoid;
        int fill = 0;
        bool isInFill = false;
        // TODO: ASSERTs for wrong segments.
        for (auto segment : *segmentList) {
            if (segment.from.y == segment.to.y)
                continue;
            if (fillRule() == EvenOdd) {
                fill = !fill;
            } else {
                fill += segment.direction;
            }

            if (fill) {
                if (!isInFill) {
                    trapezoid.bottomY = (fixPrecision(segment.topY() / denom));
                    trapezoid.topY = (fixPrecision(segment.bottomY() / denom));
                    trapezoid.bottomLeftX = (fixPrecision(segment.from.x) / denom);
                    trapezoid.topLeftX = (fixPrecision(segment.to.x) / denom);
                    if (trapezoid.bottomY != trapezoid.topY)
                        isInFill = true;
                }
            } else {
                trapezoid.bottomRightX = (fixPrecision(segment.from.x) / denom);
                trapezoid.topRightX = (fixPrecision(segment.to.x) / denom);
                if (trapezoid.bottomY != trapezoid.topY) {
                    trapezoidList.push_front(trapezoid);
                }
                isInFill = false;
            }
            ASSERT(trapezoid.bottomY == (fixPrecision(segment.topY() / denom)));
        }

        delete segmentList;

        // TODO: check the boundingBox calculation:
        // NOTE:  maxX = (maxX + (_antiAliasingLevel - 1)) / _antiAliasingLevel;
        _boundingBox.minX = (fixPrecision(segmentApproximator.boundingBox().minX) / _antiAliasingLevel);
        _boundingBox.minY = (fixPrecision(segmentApproximator.boundingBox().minY) / _antiAliasingLevel);
        _boundingBox.maxX = (fixPrecision(segmentApproximator.boundingBox().maxX) / _antiAliasingLevel);
        _boundingBox.maxY = (fixPrecision(segmentApproximator.boundingBox().maxY) / _antiAliasingLevel);
    }

    return trapezoidList;
}

} // namespace gepard
