#include "engine/graphics/Renderer.h"
#include <vector>
#include <assert.h>

Renderer::Renderer(const std::string& shaderDir)
    : m_pointShader     (shaderDir + "points.vert",     shaderDir + "points.frag")
    , m_glowShader      (shaderDir + "points.vert",     shaderDir + "glow.frag")
    , m_lineShader      (shaderDir + "line.vert",       shaderDir + "line.frag")
    , m_atmosphereShader(shaderDir + "atmosphere.vert", shaderDir + "rings.frag")
{

    assert(m_pointShader.isValid()      && "points shader failed to load");
    assert(m_glowShader.isValid()       && "glow shader failed to load");
    assert(m_lineShader.isValid()       && "line shader failed to load");
    assert(m_atmosphereShader.isValid() && "atmosphere shader failed to load");

    setupBuffers();
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glLineWidth(1.2F);
}

Renderer::~Renderer() {
    // Shader members clean up their own GL programs via RAII
    glDeleteVertexArrays(1, &m_pointsVAO);
    glDeleteBuffers(1, &m_pointsVBO);
    glDeleteVertexArrays(1, &m_trailVAO);
    glDeleteBuffers(1, &m_trailVBO);
}

void Renderer::setupBuffers() {
    glGenVertexArrays(1, &m_pointsVAO);
    glGenBuffers(1, &m_pointsVBO);

    glBindVertexArray(m_pointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_pointsVBO);

    // Layout: pos(2) size(1) color(3) hasAtm(1) atmSize(1) atmColor(3) = 11 floats
    constexpr GLsizei stride = 11 * static_cast<GLsizei>(sizeof(float));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(6 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(7 * sizeof(float)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, stride, reinterpret_cast<void*>(8 * sizeof(float)));

    glGenVertexArrays(1, &m_trailVAO);
    glGenBuffers(1, &m_trailVBO);

    glBindVertexArray(m_trailVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_trailVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE,
        2 * static_cast<GLsizei>(sizeof(float)), reinterpret_cast<void*>(0));

    glBindVertexArray(0);
}

void Renderer::uploadBodyData(const std::vector<Body>& bodies) {
    std::vector<float> data;
    data.reserve(bodies.size() * 11);

    for (const auto& body : bodies) {
        const auto gpu = body.toGPUBody();
        data.insert(data.end(), {
            gpu.x, gpu.y, gpu.size,
            gpu.r, gpu.g, gpu.b,
            gpu.hasAtmosphere, gpu.atmosphereSize,
            gpu.ar, gpu.ag, gpu.ab
        });
    }

    glBindVertexArray(m_pointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_pointsVBO);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(data.size() * sizeof(float)),
        data.data(), GL_DYNAMIC_DRAW);
}

void Renderer::beginFrame() {
    int width{0};
    int height{0};
    GLFWwindow* window = glfwGetCurrentContext();
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.01F, 0.01F, 0.02F, 1.0F);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::render(const Simulation& simulation, const Camera& camera) {
    const auto& bodies = simulation.getBodies();
    if (bodies.empty()) return;

    uploadBodyData(bodies); // upload once

    renderAtmospheres(simulation, camera);
    renderPoints(simulation, camera);
    renderTrails(simulation, camera);
}

void Renderer::renderPoints(const Simulation& simulation, const Camera& camera) {
    const auto& bodies = simulation.getBodies();

    glBindVertexArray(m_pointsVAO); // explicit bind

    const auto camX = static_cast<float>(camera.getX());
    const auto camY = static_cast<float>(camera.getY());

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    m_glowShader.use();
    m_glowShader.setFloat("uScale", camera.getScale());
    m_glowShader.setVec2("uCam", camX, camY);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(bodies.size()));

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_pointShader.use();
    m_pointShader.setFloat("uScale", camera.getScale());
    m_pointShader.setVec2("uCam", camX, camY);
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(bodies.size()));

    glBindVertexArray(0);
}

void Renderer::renderAtmospheres(const Simulation& simulation, const Camera& camera) {
    const auto& bodies = simulation.getBodies();

    glBindVertexArray(m_pointsVAO); // explicit bind

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_atmosphereShader.use();
    m_atmosphereShader.setFloat("uScale", camera.getScale());
    m_atmosphereShader.setVec2("uCam",
        static_cast<float>(camera.getX()),
        static_cast<float>(camera.getY()));
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(bodies.size()));

    glBindVertexArray(0);
}

void Renderer::renderTrails(const Simulation& simulation, const Camera& camera) {
    const auto& bodies = simulation.getBodies();

    m_lineShader.use();
    m_lineShader.setFloat("uScale", camera.getScale());
    m_lineShader.setVec2("uCam",
        static_cast<float>(camera.getX()),
        static_cast<float>(camera.getY()));

    glBindVertexArray(m_trailVAO);

    for (const auto& body : bodies) {
        const auto& trail = body.getTrail();
        if (trail.size() >= 4) {
            glBindBuffer(GL_ARRAY_BUFFER, m_trailVBO);
            glBufferData(GL_ARRAY_BUFFER,
                static_cast<GLsizeiptr>(trail.size() * sizeof(float)),
                trail.data(), GL_DYNAMIC_DRAW);
            const auto& col = body.getColor();
            m_lineShader.setVec3("uColor", col[0] * 0.8F, col[1] * 0.8F, col[2] * 0.8F);
            m_lineShader.setFloat("uAlpha", 1.0f);
            glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(trail.size() / 2));
        }

        for (const auto& moon : body.getMoons()) {
            const auto& moonTrail = moon.getTrail();
            if (moonTrail.size() < 4) { continue; }

            glBindBuffer(GL_ARRAY_BUFFER, m_trailVBO);
            glBufferData(GL_ARRAY_BUFFER,
                static_cast<GLsizeiptr>(moonTrail.size() * sizeof(float)),
                moonTrail.data(), GL_DYNAMIC_DRAW);
            const auto& moonCol = moon.getColor();
            m_lineShader.setVec3("uColor", moonCol[0] * 0.6F, moonCol[1] * 0.6F, moonCol[2] * 0.6F);
            glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(moonTrail.size() / 2));
        }
    }

    glBindVertexArray(0);
}

void Renderer::endFrame() {}