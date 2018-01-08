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

#define _GD_GLES2_SHADER_PROGRAM_STR(...)  #__VA_ARGS__
#define GD_GLES2_SHADER_PROGRAM(...) _GD_GLES2_SHADER_PROGRAM_STR(__VA_ARGS__)

namespace gepard {
namespace gles2 {

/*!
 * \brief The ShaderProgram struct
 */
struct ShaderProgram {
    static constexpr GLuint kInvalidProgramID = GLuint(-1);

    ShaderProgram() : id(kInvalidProgramID) {}
    ~ShaderProgram()
    {
        glDeleteProgram(id);
    }

    /*!
     * \brief Function to compile a shader program.
     * \param vertexShaderSource  vertex shader source code.
     * \param fragmentShaderSource  fragment shader source code.
     * \param name  the name of the program.
     *
     * \internal
     */
    void compileShaderProgram(const std::string& name, const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
    const bool isInvalid() const { return id == kInvalidProgramID; }

    GLuint id;
protected:
    static void logShaderCompileError(const GLuint shader);
    static void logProgramLinkError(const GLuint program);
    static const GLuint compileShader(const GLenum type, const GLchar* shaderSource);
    static const GLuint linkPrograms(const GLuint vertexShader, const GLuint fragmentShader);
};

/*!
 * \brief The ShaderProgramManager class
 */
class ShaderProgramManager {
public:
    ShaderProgram& getProgram(const std::string& name, const std::string& vertexShaderSource, const std::string& fragmentShaderSource);

private:
    std::map<std::string, ShaderProgram> _programs;
};

} // namespace gles2
} // namespace gepard

#endif // GEPARD_GLES2_SHADER_FACTORY_H

#endif // GD_USE_GLES2
