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
    void render(const Simulation& simulation, const Camera& camera);
    void endFrame();

private:
    // Shaders — RAII
    Shader m_pointShader;
    Shader m_glowShader;
    Shader m_lineShader;
    Shader m_atmosphereShader;

    // Buffers
    GLuint m_pointsVAO{0};
    GLuint m_pointsVBO{0};
    GLuint m_trailVAO{0};
    GLuint m_trailVBO{0};

    void setupBuffers();
    void uploadBodyData(const std::vector<Body>& bodies);
    void renderPoints(const Simulation& simulation, const Camera& camera);
    void renderAtmospheres(const Simulation& simulation, const Camera& camera);
    void renderTrails(const Simulation& simulation, const Camera& camera);
};