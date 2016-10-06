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

#ifdef GD_USE_GLES2

#ifndef GEPARD_GLES2_SHADER_FACTORY_H
#define GEPARD_GLES2_SHADER_FACTORY_H

#include "gepard-defs.h"

#include "gepard-gles2-defs.h"
#include <map>
#include <string>

namespace gepard {
namespace gles2 {

/*!
 * \brief The ShaderProgram class
 */
struct ShaderProgram {
    ShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource, const std::string& name = "")
        : id(0)
        , _vertexShaderSource(vertexShaderSource)
        , _fragmentShaderSource(fragmentShaderSource)
        , _name(name)
    {
    }
    /*!
     * \brief Function to compile a shader program.
     * \param result  pointer of the number of compiled program. If it's not nullptr then nothing happens.
     * \param name  the name of the program (only for the logging at the moment).
     * \param vertexShaderSource  vertex shader source code.
     * \param fragmentShaderSource  fragment shader source code.
     *
     * \internal
     */
    void compileShaderProgram();

    uint32_t hash;
    uint32_t id;
protected:
    static void logShaderCompileError(const GLuint shader);
    static void logProgramLinkError(const GLuint program);
    static GLuint compileShader(const GLenum type, const GLchar* shaderSource);
    static GLuint linkPrograms(GLuint vertexShader, GLuint fragmentShader);
private:
    const std::string _vertexShaderSource;
    const std::string _fragmentShaderSource;
    const std::string _name;
};

} // namespace gles2
} // namespace gepard

#endif // GEPARD_GLES2_SHADER_FACTORY_H

#endif // GD_USE_GLES2
