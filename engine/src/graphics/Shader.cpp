#include "engine/graphics/Shader.h"
#include "engine/core/Logger.h"
#include "glad/gl.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    GLuint vertexShader   = compileShader(vertexPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentPath, GL_FRAGMENT_SHADER);

    if (vertexShader == 0 || fragmentShader == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return; // mId stays 0, isVaild() return false
    }

    m_programId = glCreateProgram();
    glAttachShader(m_programId, vertexShader);
    glAttachShader(m_programId, fragmentShader);
    glLinkProgram(m_programId);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!checkLink(m_programId)) {
        glDeleteProgram(m_programId);
        m_programId = 0;
    }
}

Shader::~Shader() {
    if (m_programId != 0)
        glDeleteProgram(m_programId);
}

Shader::Shader(Shader&& other) noexcept
    : m_programId(other.m_programId)
    , m_uniformCache(std::move(other.m_uniformCache)) {
        other.m_programId = 0; // save no-op
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (m_programId != 0)
            glDeleteProgram(m_programId);

        m_programId = other.m_programId;
        m_uniformCache = std::move(other.m_uniformCache);
        other.m_programId = 0;
    }

    return *this;
}

void Shader::use() const {
    if (m_programId == 0) {
        LOG_WARN("Shader", "Attemped to use invalid program");
        return;
    }
    
    glUseProgram(m_programId);
}

void Shader::setFloat(const std::string& name, float value) {
    if (m_programId == 0) return;
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec2(const std::string& name, float x, float y) {
    glUniform2f(getUniformLocation(name), x, y);
}

void Shader::setVec3(const std::string& name, float x, float y, float z) {
    GLint loc = getUniformLocation(name);
    if (loc == -1) return; 
    glUniform3f(loc, x, y, z);
}

GLint Shader::getUniformLocation(const std::string& name) {
    auto it = m_uniformCache.find(name);

    if (it != m_uniformCache.end())
        return it->second;

    GLint location = glGetUniformLocation(m_programId, name.c_str());
    m_uniformCache[name] = location;
    return location;
}



std::string Shader::loadFile(const std::string& path) {
    std::ifstream file(path);

    if (!file.is_open()) {
        LOG_ERROR("Shader", "Cannot open file: " + path);
        return {};
    }

    std::ostringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

GLuint Shader::compileShader(const std::string& path, GLenum type) {
    std::string source = loadFile(path);

    if (source.empty())
        return 0;

    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    if (!checkCompile(shader, path)) {
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

bool Shader::checkCompile(GLuint shader, const std::string& path) {
    GLint success{0};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (success == 0) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        LOG_ERROR("Shader", "Compile error in [" + path + "]:\n" + infoLog);
        return false;
    }

    return true;
}

bool Shader::checkLink(GLuint program) {
    GLint success{0};
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (success == 0) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        LOG_ERROR("Shader", "Link error:\n" + std::string(infoLog));
        return false;
    }
    return true;
}