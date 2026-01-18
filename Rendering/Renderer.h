#pragma once

#include <glad/glad.h> 
#include <GLFW/glfw3.h>

#include "Simulation.h"
#include "Camera.h"
#include <memory>

class Renderer {
private:
    GLuint m_pointProgram, m_glowProgram, m_lineProgram, m_atmosphereProgram;
    GLuint m_pointsVAO, m_pointsVBO, m_trailVAO, m_trailVBO;

    void compileShaders();
    void setupBuffers();
    void renderPoints(const Simulation& simulation, const Camera& camera);
    void renderTrails(const Simulation& simulation, const Camera& camera);
    void renderAtmospheres(const Simulation& simulation, const Camera& camera);
    void renderMoons(const Simulation& simulation, const Camera& camera);

public:
    Renderer();
    ~Renderer();

    void beginFrame();
    void render(const Simulation& simulation, const Camera& camera);
    void endFrame();
};