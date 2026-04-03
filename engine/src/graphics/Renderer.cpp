#include "engine/graphics/Renderer.h"
#include <vector>
#include <random>
#include <assert.h>

Renderer::Renderer(const std::string& shaderDir)
    : m_pointShader     (shaderDir + "points.vert",     shaderDir + "points.frag")
    , m_glowShader      (shaderDir + "points.vert",     shaderDir + "glow.frag")
    , m_lineShader      (shaderDir + "line.vert",       shaderDir + "line.frag")
    , m_atmosphereShader(shaderDir + "atmosphere.vert", shaderDir + "rings.frag")
    , m_sunShader       (shaderDir + "points.vert", shaderDir + "sun.frag")
    , m_starShader      (shaderDir + "star.vert",   shaderDir + "star.frag")
{

    assert(m_pointShader.isValid()      && "points shader failed to load");
    assert(m_glowShader.isValid()       && "glow shader failed to load");
    assert(m_lineShader.isValid()       && "line shader failed to load");
    assert(m_atmosphereShader.isValid() && "atmosphere shader failed to load");
    assert(m_sunShader.isValid()        && "sun shader failed to load");
    assert(m_starShader.isValid()       && "star shader failed to load");

    setupBuffers();
    setupStars();
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glLineWidth(1.0F);
}

Renderer::~Renderer() {
    // Shader members clean up their own GL programs via RAII
    glDeleteVertexArrays(1, &m_pointsVAO);
    glDeleteBuffers(1, &m_pointsVBO);
    glDeleteVertexArrays(1, &m_trailVAO);
    glDeleteBuffers(1, &m_trailVBO);
    glDeleteVertexArrays(1, &m_starVAO);
    glDeleteBuffers(1, &m_starVBO);
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

    // Layout: pos(2) progress(1) = 3 floats per vertex
    constexpr GLsizei trailStride = 3 * static_cast<GLsizei>(sizeof(float));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, trailStride, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, trailStride, reinterpret_cast<void*>(2 * sizeof(float)));

    glBindVertexArray(0);
}

void Renderer::setupStars() {
    // Fixed seed — same stars every run
    std::mt19937 rng(12345);
    std::uniform_real_distribution<float> pos(-1.0F, 1.0F);
    std::uniform_real_distribution<float> brt(0.25F, 1.0F);

    constexpr int COUNT = 2500;
    std::vector<float> data;
    data.reserve(COUNT * 3);
    for (int i = 0; i < COUNT; ++i) {
        data.push_back(pos(rng));  // x  (NDC)
        data.push_back(pos(rng));  // y  (NDC)
        data.push_back(brt(rng));  // brightness
    }
    m_starCount = COUNT;

    glGenVertexArrays(1, &m_starVAO);
    glGenBuffers(1, &m_starVBO);

    glBindVertexArray(m_starVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_starVBO);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(data.size() * sizeof(float)),
        data.data(), GL_STATIC_DRAW);  // stars never move

    constexpr GLsizei starStride = 3 * static_cast<GLsizei>(sizeof(float));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, starStride, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, starStride, reinterpret_cast<void*>(2 * sizeof(float)));

    glBindVertexArray(0);
}

void Renderer::renderStars(float time) {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    m_starShader.use();
    m_starShader.setFloat("uTime", time);
    glBindVertexArray(m_starVAO);
    glDrawArrays(GL_POINTS, 0, m_starCount);
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

void Renderer::render(const Simulation& simulation, const Camera& camera, float time) {
    const auto& bodies = simulation.getBodies();
    if (bodies.empty()) return;

    uploadBodyData(bodies); // upload once

    renderStars(time);                    // background stars
    renderAtmospheres(simulation, camera, time); // planet halos
    renderPoints(simulation, camera, time); // planet bodies
    renderTrails(simulation, camera);      // orbit trails
}

void Renderer::renderPoints(const Simulation& simulation, const Camera& camera, float time) {
    const auto& bodies = simulation.getBodies();

    glBindVertexArray(m_pointsVAO); // explicit bind

    const auto camX = static_cast<float>(camera.getX());
    const auto camY = static_cast<float>(camera.getY());

    // --- Sun (body 0) — animated shader ---
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    m_sunShader.use();
    m_sunShader.setFloat("uScale", camera.getScale());
    m_sunShader.setVec2("uCam", camX, camY);
    m_sunShader.setFloat("uTime", time);
    glDrawArrays(GL_POINTS, 0, 1);

    // --- All other bodies (1..N) ---
    const GLsizei rest = static_cast<GLsizei>(bodies.size() - 1);

    const auto sunX = static_cast<float>(bodies[0].getPosition().x);
    const auto sunY = static_cast<float>(bodies[0].getPosition().y);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    m_glowShader.use();
    m_glowShader.setFloat("uScale", camera.getScale());
    m_glowShader.setVec2("uCam", camX, camY);
    glDrawArrays(GL_POINTS, 1, rest);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_pointShader.use();
    m_pointShader.setFloat("uScale", camera.getScale());
    m_pointShader.setFloat("uTime",  time);
    m_pointShader.setVec2("uCam", camX, camY);
    m_pointShader.setVec2("uSunPos", sunX, sunY);
    glDrawArrays(GL_POINTS, 1, rest);

    glBindVertexArray(0);
}

void Renderer::renderAtmospheres(const Simulation& simulation, const Camera& camera, float time) {
    const auto& bodies = simulation.getBodies();

    glBindVertexArray(m_pointsVAO); // explicit bind

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);  // additive — halos emit, don't paint
    m_atmosphereShader.use();
    m_atmosphereShader.setFloat("uScale", camera.getScale());
    m_atmosphereShader.setFloat("uTime",  time);
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

    auto uploadTrail = [&](const std::vector<float>& raw) -> int {
        const int n = static_cast<int>(raw.size() / 2);
        if (n < 2) return 0;

        std::vector<float> data;
        data.reserve(n * 3);
        for (int i = 0; i < n; ++i) {
            float progress = static_cast<float>(i) / static_cast<float>(n - 1);
            data.push_back(raw[i * 2]);
            data.push_back(raw[i * 2 + 1]);
            data.push_back(progress);       // 0.0 = tail, 1.0 = near planet
        }

        glBindBuffer(GL_ARRAY_BUFFER, m_trailVBO);
        glBufferData(GL_ARRAY_BUFFER,
            static_cast<GLsizeiptr>(data.size() * sizeof(float)),
            data.data(), GL_DYNAMIC_DRAW);
        return n;
    };

    for (const auto& body : bodies) {
        const int n = uploadTrail(body.getTrail());
        if (n >= 2) {
            const auto& col = body.getColor();
            m_lineShader.setVec3("uColor", col[0] * 0.8F, col[1] * 0.8F, col[2] * 0.8F);
            m_lineShader.setFloat("uAlpha", 1.0F);
            glDrawArrays(GL_LINE_STRIP, 0, n);
        }

        for (const auto& moon : body.getMoons()) {
            const int mn = uploadTrail(moon.getTrail());
            if (mn >= 2) {
                const auto& moonCol = moon.getColor();
                m_lineShader.setVec3("uColor", moonCol[0] * 0.6F, moonCol[1] * 0.6F, moonCol[2] * 0.6F);
                m_lineShader.setFloat("uAlpha", 0.7F);
                glDrawArrays(GL_LINE_STRIP, 0, mn);
            }
        }
    }

    glBindVertexArray(0);
}

void Renderer::endFrame() {}