#pragma once

#include <glad/gl.h>
#include <string>
#include <unordered_map> 


class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    // noncopyable, movable
    Shader(const Shader&)               = delete;
    Shader& operator=(const Shader&)    = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void use() const;

    // Uniforms setters
    void setFloat(const std::string& name, float value);
    void setVec2(const std::string& name, float x, float y);
    void setVec3(const std::string& name, float x, float y, float z);

    bool isValid() const { return m_programId != 0; }

private:
    GLuint m_programId{0};
    std::unordered_map<std::string, GLint> m_uniformCache;

    GLint getUniformLocation(const std::string& name);

    // static helpers
    static GLuint compileShader(const std::string& path, GLenum type);
    static std::string loadFile(const std::string& path);
    static bool checkCompile(GLuint shader, const std::string& path);
    static bool checkLink(GLuint program);
};