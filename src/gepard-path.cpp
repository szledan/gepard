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

#include "gepard-trapezoid-tessellator.h"
#include <list>
#include <map>
#include <math.h>
#include <memory>
#include <set>

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
        gl_FragColor = vec4(u_color.rgb, u_color.a * texture2D(u_texture, v_texturePosition).a);
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
    GLfloat slopeLeft = (trapezoid.bottomLeftX - trapezoid.topLeftX) / (trapezoid.bottomY - trapezoid.topY);
    GLfloat slopeRight = (trapezoid.bottomRightX - trapezoid.topRightX) / (trapezoid.bottomY - trapezoid.topY);
    const GLfloat bottomY = floor(trapezoid.topY);
    const GLfloat topY = ceil(trapezoid.bottomY);
    // The fraction is stored in a temporary variable.
    GLfloat temp, x1, x2;
    GLfloat bottomLeftX, bottomRightX, topLeftX, topRightX;

    temp = ceil(trapezoid.bottomY) - (trapezoid.bottomY);
    x1 = trapezoid.bottomLeftX - temp * slopeLeft;
    x2 = trapezoid.bottomRightX - temp * slopeRight;
    topLeftX = floor(x1 - fabs(slopeLeft));
    topRightX = ceil(x2 + fabs(slopeRight));

    temp = trapezoid.topY - floor(trapezoid.topY);
    x1 = trapezoid.topLeftX - temp * slopeLeft;
    x2 = trapezoid.topRightX - temp * slopeRight;
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

        *attributes++ = (trapezoid.topY);
        *attributes++ = (trapezoid.bottomY);

        *attributes++ = (x1);
        *attributes++ = (x2);
        *attributes++ = (slopeLeft);
        *attributes++ = (slopeRight);
    }
}

/* PathData */

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
    FloatPoint start = FloatPoint(center.x + cos(startAngle) * radius.x, center.y + sin(startAngle) * radius.y);

    if (!_lastElement) {
        addMoveToElement(center);
    }

    if (!radius.x || !radius.x || startAngle == endAngle) {
        addLineToElement(start);
        return;
    }

    if (_lastElement->to != start) {
        addLineToElement(start);
    }

    const Float twoPiFloat = 2.0 * piFloat;
    if (antiClockwise && startAngle - endAngle >= twoPiFloat) {
        startAngle = fmod(startAngle, twoPiFloat);
        endAngle = startAngle - twoPiFloat;
    } else if (!antiClockwise && endAngle - startAngle >= twoPiFloat) {
        startAngle = fmod(startAngle, twoPiFloat);
        endAngle = startAngle + twoPiFloat;
    } else {
        bool equal = startAngle == endAngle;

        startAngle = fmod(startAngle, twoPiFloat);
        if (startAngle < 0) {
            startAngle += twoPiFloat;
        }

        endAngle = fmod(endAngle, twoPiFloat);
        if (endAngle < 0) {
            endAngle += twoPiFloat;
        }

        if (!antiClockwise) {
            if (startAngle > endAngle || (startAngle == endAngle && !equal)) {
                endAngle += twoPiFloat;
            }
            ASSERT(0 <= startAngle && startAngle <= twoPiFloat);
            ASSERT(startAngle <= endAngle && endAngle - startAngle <= twoPiFloat);
        } else {
            if (startAngle < endAngle || (startAngle == endAngle && !equal)) {
                endAngle -= twoPiFloat;
            }
            ASSERT(0 <= startAngle && startAngle <= twoPiFloat);
            ASSERT(endAngle <= startAngle && startAngle - endAngle <= twoPiFloat);
        }
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

/* Path */

void Path::fillPath(const std::string fillRuleStr)
{
    ASSERT(_pathData.lastElement()->isCloseSubpath());

    _pathData.dump();

    if (!_surface)
        return;

    // 0. Parse fill rule.
    TrapezoidTessellator::FillRule fillRule = TrapezoidTessellator::FillRule::NonZero;
    if (fillRuleStr.compare("evenodd") == 0)
        fillRule = TrapezoidTessellator::FillRule::EvenOdd;

    // 1. Tessellating the path.
    TrapezoidTessellator tt(this, fillRule, ANTIALIAS_LEVEL);
    TrapezoidList trapezoidList = tt.trapezoidList();

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
    std::cout << "Trapezoids (" << trapezoidList.size() << ")" << std::endl;
    int trapezoidIndex = 0;
    for (Trapezoid trapezoid : trapezoidList) {
        ASSERT(trapezoid.topY < trapezoid.bottomY);
        ASSERT(trapezoid.topLeftX <= trapezoid.topRightX);
        ASSERT(trapezoid.bottomLeftX <= trapezoid.bottomRightX);

        if (!trapezoid.leftId || !trapezoid.rightId)
            continue;

//        std::cout << trapezoid << std::endl;
        setupAttributes(trapezoid, attributes + trapezoidIndex * 32, tt.antiAliasingLevel());
        trapezoidIndex++;
        if (trapezoidIndex >= min(s_kMaximumNumberOfUshortQuads, s_kMaximumNumberOfAttributes / 32)) {
            glDrawElements(GL_TRIANGLES, 6 * trapezoidIndex, GL_UNSIGNED_SHORT, nullptr);
            trapezoidIndex = 0;
        }
    }

    if (trapezoidIndex) {
        glDrawElements(GL_TRIANGLES, 6 * trapezoidIndex, GL_UNSIGNED_SHORT, nullptr);
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
    static GLfloat textureCoords[] = {
        0, 512, 0, 0,
        0, 0, 0, 1,
        512, 512, 1, 0,
        512, 0, 1, 1
    };

    intValue = glGetAttribLocation(copyPathShader, "a_position");
    glEnableVertexAttribArray(intValue);
    glVertexAttribPointer(intValue, 4, GL_FLOAT, GL_FALSE, 0, textureCoords);

    intValue = glGetUniformLocation(copyPathShader, "u_texture");
    glBindTexture(GL_TEXTURE_2D, pathTexture);

    // 3.f Set color of path.
    intValue = glGetUniformLocation(copyPathShader, "u_color");
    glUniform4f(intValue, 0.0f, 0.4f, 0.7f, 1.0f);

    // 3.g Copy path to display.
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // 4. Drawing is ready. Swap buffers.
    eglSwapBuffers(_surface->eglDisplay(), _surface->eglSurface());

    // TODO: Remove this line. (only testing)
    printf("glGetError: %d\n", glGetError());

    free(attributes);
}

} // namespace gepard
