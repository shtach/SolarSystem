#pragma once

#include <glad/gl.h> 
#include <GLFW/glfw3.h>
#include <string>

#include "engine/physics/Simulation.h"
#include "engine/graphics/Camera.h"
#include "engine/graphics/Shader.h"

class Renderer {
public:
    explicit Renderer(const std::string& shaderDir = "assets/shaders/");
    ~Renderer();

    Renderer(const Renderer&)            = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&)                 = delete;
    Renderer& operator=(Renderer&&)      = delete;

    void beginFrame();
    void render(const Simulation& simulation, const Camera& camera, float time);
    void endFrame();

private:
    // Shaders — RAII
    Shader m_pointShader;
    Shader m_glowShader;
    Shader m_lineShader;
    Shader m_atmosphereShader;
    Shader m_sunShader;
    Shader m_starShader;

    // Buffers
    GLuint m_pointsVAO{0};
    GLuint m_pointsVBO{0};
    GLuint m_trailVAO{0};
    GLuint m_trailVBO{0};
    GLuint m_starVAO{0};
    GLuint m_starVBO{0};
    int    m_starCount{0};

    // Textures
    GLuint m_texArray{0};
    static constexpr int TEX_COUNT = 12;
    static constexpr int TEX_SIZE  = 540;

    void setupBuffers();
    void setupStars();
    void loadTextures(const std::string& texDir);
    void uploadBodyData(const std::vector<Body>& bodies);
    void renderStars(float time);
    void renderPoints(const Simulation& simulation, const Camera& camera, float time);
    void renderAtmospheres(const Simulation& simulation, const Camera& camera, float time);
    void renderTrails(const Simulation& simulation, const Camera& camera);
};