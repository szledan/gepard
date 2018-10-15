/* Copyright (C) 2016-2018, Gepard Graphics
 * Copyright (C) 2016-2018, Szilard Ledan <szledan@gmail.com>
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

#include "gepard-gles2-shader-factory.h"

#include "gepard-defs.h"
#include "gepard-gles2-defs.h"
#include "gepard-logging.h"
#include <string>
#include <vector>

namespace gepard {
namespace gles2 {

// ShaderProgram

void ShaderProgram::logShaderCompileError(const GLuint shader)
{
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> errorLog(maxLength);
    GLsizei length = 0;
    glGetShaderInfoLog(shader, maxLength, &length, &errorLog[0]);

    GD_LOG(ERROR) << "Shader compilation failed with: " << std::string(errorLog.begin(), errorLog.end());
}

void ShaderProgram::logProgramLinkError(const GLuint program)
{
    GLint maxLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> errorLog(maxLength);
    GLsizei length = 0;
    glGetProgramInfoLog(program, maxLength, &length, &errorLog[0]);

    GD_LOG(ERROR) << "Shader program link failed with: " << std::string(errorLog.begin(), errorLog.end());
}

const GLuint ShaderProgram::compileShader(const GLenum type, const GLchar* shaderSource)
{
    GD_LOG(DEBUG) << "Compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader program";

    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    GLint isCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

    if (isCompiled != GL_TRUE) {
        logShaderCompileError(shader);
        return kInvalidProgramID;
    }

    return shader;
}

const GLuint ShaderProgram::linkPrograms(const GLuint vertexShader, const GLuint fragmentShader)
{
    GD_LOG(DEBUG) << "Link shader programs; vertex shader: " << vertexShader << ", fragment shader: " << fragmentShader;
    GD_ASSERT(vertexShader && "Vertex shader program doesn't exist!");
    GD_ASSERT(fragmentShader && "Fragment shader program doesn't exist!");

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint isLinked;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked != GL_TRUE) {
        logProgramLinkError(program);
        glDeleteProgram(program);
        return kInvalidProgramID;
    }
    return program;
}

void ShaderProgram::compileShaderProgram(const std::string& name, const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
    if (id != kInvalidProgramID)
        return;

    GD_LOG(INFO) << "Compile '" << name << "' shader program.";

    GLuint vertexShader = 0;
    vertexShader = compileShader(GL_VERTEX_SHADER, (const GLchar*)vertexShaderSource.c_str());

    GLuint fragmentShader = 0;
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, (const GLchar*)fragmentShaderSource.c_str());

    if (vertexShader && fragmentShader) {
        id = linkPrograms(vertexShader, fragmentShader);
        GD_LOG(DEBUG) << "The '" << name << "' linked program is: " << id;
    }

    // According to the specification, the shaders are kept
    // around until the program object is freed (reference counted).
    if (vertexShader) {
        glDeleteShader(vertexShader);
    }
    if (fragmentShader) {
        glDeleteShader(fragmentShader);
    }
}

ShaderProgram& ShaderProgramManager::getProgram(const std::string& name, const std::string& vertexShaderSource, const std::string& fragmentShaderSource)
{
    ShaderProgram& program = _programs[name];
    if (program.isInvalid()) {
        program.compileShaderProgram(name, vertexShaderSource, fragmentShaderSource);
        GD_LOG(DEBUG) << "Add new shader program: " << name << " with ID: " << program.id;
    }

    return program;
}

} // namespace gles2
} // namespace gepard
