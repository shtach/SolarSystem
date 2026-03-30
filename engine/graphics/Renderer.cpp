#include "Renderer.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>

// Shader sources
const char* vs_points = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in float aSize;
layout(location = 2) in vec3 aColor;

out vec3 vColor;
uniform vec2 uCam;
uniform float uScale;

void main() {
    vec2 p = (aPos - uCam) * uScale;
    gl_Position = vec4(p, 0.0, 1.0);
    gl_PointSize = aSize;
    vColor = aColor;
}
)";

const char* fs_points = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;

void main() {
    vec2 c = gl_PointCoord - vec2(0.5);
    float d = length(c);
    if(d > 0.5) discard;
    float alpha = smoothstep(0.5, 0.45, d);
    FragColor = vec4(vColor, alpha);
}
)";

const char* vs_line = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
uniform vec2 uCam;
uniform float uScale;

void main() {
    vec2 p = (aPos - uCam) * uScale;
    gl_Position = vec4(p, 0.0, 1.0);
}
)";

const char* fs_line = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 uColor;

void main() {
    FragColor = vec4(uColor, 1.0);
}
)";

const char* fs_glow = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;

void main() {
    float d = length(gl_PointCoord - vec2(0.5));
    if(d > 0.6) discard;
    float alpha = (1.0 - smoothstep(0.0, 0.6, d));
    alpha = pow(alpha, 0.6);
    FragColor = vec4(vColor * 2.0, alpha);
}
)";

const char* vs_atmosphere = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in float aSize;
layout(location = 2) in vec3 aColor;
layout(location = 3) in float aHasAtmosphere;
layout(location = 4) in float aAtmosphereSize;
layout(location = 5) in vec3 aAtmosphereColor;

out vec3 vColor;
out vec3 vAtmosphereColor;
out float vAtmosphereSize;
out float vHasAtmosphere;
out float vSize;

uniform vec2 uCam;
uniform float uScale;

void main() {
    vec2 p = (aPos - uCam) * uScale;
    gl_Position = vec4(p, 0.0, 1.0);
    gl_PointSize = aSize * aAtmosphereSize;
    vColor = aColor;
    vAtmosphereColor = aAtmosphereColor;
    vAtmosphereSize = aAtmosphereSize;
    vHasAtmosphere = aHasAtmosphere;
    vSize = aSize;
}
)";

const char* fs_atmosphere = R"(
#version 330 core
in vec3 vColor;
in vec3 vAtmosphereColor;
in float vAtmosphereSize;
in float vHasAtmosphere;
in float vSize;

out vec4 FragColor;

void main() {
    if (vHasAtmosphere < 0.5) discard;
    
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    
    // Promień planety w przestrzeni tekstury
    float planetRadius = 0.5 / vAtmosphereSize;
    
    // Rysujemy tylko atmosferę (od planetRadius do 0.5)
    if (dist < planetRadius || dist > 0.5) discard;
    
    // Gradient atmosfery - bardziej realistyczny
    float innerEdge = planetRadius;
    float outerEdge = 0.5;
    
    // Dwa gradienty: gęsty wewnątrz, rozrzedzony na zewnątrz
    float alpha1 = 1.0 - smoothstep(innerEdge, outerEdge * 0.7, dist);
    float alpha2 = 1.0 - smoothstep(outerEdge * 0.7, outerEdge, dist);
    float alpha = (alpha1 * 0.8 + alpha2 * 0.2) * 0.4;
    
    // Dla Słońca - jaśniejsza atmosfera
    if (vSize > 35.0) {
        alpha *= 2.0;
    }
    
    FragColor = vec4(vAtmosphereColor, alpha);
}
)";

const char* fs_rings = R"(
#version 330 core
in vec3 vColor;
in vec3 vAtmosphereColor;
in float vAtmosphereSize;
in float vHasAtmosphere;
in float vSize;

out vec4 FragColor;

void main() {
    if (vHasAtmosphere < 0.5) discard;
    
    vec2 coord = gl_PointCoord - vec2(0.5);
    float dist = length(coord);
    
    float planetRadius = 0.5 / vAtmosphereSize;
    
    // Dla Saturna - symulacja pierścieni
    if (vSize > 15.0 && vSize < 20.0) { // Saturn
        // Pierścienie jako koncentryczne okręgi
        if (dist > planetRadius * 1.2 && dist < planetRadius * 2.0) {
            float ringPattern = mod(dist * 20.0, 1.0);
            if (ringPattern > 0.3) {
                float alpha = 0.6 * (1.0 - smoothstep(planetRadius * 1.2, planetRadius * 2.0, dist));
                FragColor = vec4(0.9, 0.8, 0.6, alpha);
                return;
            }
        }
    }
    
    // Normalna atmosfera dla innych planet
    if (dist < planetRadius || dist > 0.5) discard;
    
    float innerEdge = planetRadius;
    float outerEdge = 0.5;
    float alpha1 = 1.0 - smoothstep(innerEdge, outerEdge * 0.7, dist);
    float alpha2 = 1.0 - smoothstep(outerEdge * 0.7, outerEdge, dist);
    float alpha = (alpha1 * 0.8 + alpha2 * 0.2) * 0.4;
    
    if (vSize > 35.0) {
        alpha *= 2.0;
    }
    
    FragColor = vec4(vAtmosphereColor, alpha);
}
)";

const char* fs_trail = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 uColor;
uniform float uAlpha;

void main() {
    FragColor = vec4(uColor, uAlpha * 0.7); // Przezroczyste ślady
}
)";

// Helper functions for shader compilation
static bool checkCompile(GLuint shader, const std::string& name = "") {
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation error (" << name << "):\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

static bool checkLink(GLuint program) {
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Program linking error:\n" << infoLog << std::endl;
        return false;
    }
    return true;
}

GLuint createProgram(const char* vertexSrc, const char* fragmentSrc) {
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, nullptr);
    glCompileShader(vertexShader);
    checkCompile(vertexShader, "vertex");

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, nullptr);
    glCompileShader(fragmentShader);
    checkCompile(fragmentShader, "fragment");

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    checkLink(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

Renderer::Renderer() {
    compileShaders();
    setupBuffers();

    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Renderer::~Renderer() {
    glDeleteProgram(m_pointProgram);
    glDeleteProgram(m_glowProgram);
    glDeleteProgram(m_lineProgram);
    glDeleteProgram(m_atmosphereProgram);
    glDeleteVertexArrays(1, &m_pointsVAO);
    glDeleteBuffers(1, &m_pointsVBO);
    glDeleteVertexArrays(1, &m_trailVAO);
    glDeleteBuffers(1, &m_trailVBO);
}

void Renderer::compileShaders() {
    m_pointProgram = createProgram(vs_points, fs_points);
    m_glowProgram = createProgram(vs_points, fs_glow);
    m_lineProgram = createProgram(vs_line, fs_line);
    m_atmosphereProgram = createProgram(vs_atmosphere, fs_rings); // Używamy shadera z pierścieniami
}


void Renderer::setupBuffers() {
    // Setup points VAO/VBO - rozszerzony o dane atmosfery
    glGenVertexArrays(1, &m_pointsVAO);
    glGenBuffers(1, &m_pointsVBO);
    
    glBindVertexArray(m_pointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_pointsVBO);
    
    // Position (2), Size (1), Color (3), HasAtmosphere (1), AtmosphereSize (1), AtmosphereColor (3)
    // Razem: 11 floatów na wierzchołek
    GLsizei stride = 11 * sizeof(float);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, stride, (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));

    // Setup trails VAO/VBO (bez zmian)
    glGenVertexArrays(1, &m_trailVAO);
    glGenBuffers(1, &m_trailVBO);
    
    glBindVertexArray(m_trailVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_trailVBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    
    glBindVertexArray(0);
}

void Renderer::beginFrame() {
    int width, height;
    GLFWwindow* window = glfwGetCurrentContext();
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glClearColor(0.01f, 0.01f, 0.02f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::render(const Simulation& simulation, const Camera& camera) {
    renderAtmospheres(simulation, camera); // Najpierw atmosfery (w tle)
    renderPoints(simulation, camera);      // Potem wszystkie ciała (planety i księżyce)
    renderTrails(simulation, camera);      // Na końcu tory
}

void Renderer::renderPoints(const Simulation& simulation, const Camera& camera) {
    const auto& bodies = simulation.getBodies();
    if (bodies.empty()) return;

    // Przygotuj dane dla GPU (tylko podstawowe dane)
    std::vector<float> pointData;
    for (const auto& body : bodies) {
        auto gpuBody = body.toGPUBody();
        pointData.push_back(gpuBody.x);
        pointData.push_back(gpuBody.y);
        pointData.push_back(gpuBody.size);
        pointData.push_back(gpuBody.r);
        pointData.push_back(gpuBody.g);
        pointData.push_back(gpuBody.b);
        pointData.push_back(gpuBody.hasAtmosphere);
        pointData.push_back(gpuBody.atmosphereSize);
        pointData.push_back(gpuBody.ar);
        pointData.push_back(gpuBody.ag);
        pointData.push_back(gpuBody.ab);
    }

    // Upload danych i renderuj z efektem glow
    glBindVertexArray(m_pointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_pointsVBO);
    glBufferData(GL_ARRAY_BUFFER, pointData.size() * sizeof(float), pointData.data(), GL_DYNAMIC_DRAW);

    // Renderuj glow (additive blending)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glUseProgram(m_glowProgram);
    GLuint glowScaleLoc = glGetUniformLocation(m_glowProgram, "uScale");
    GLuint glowCamLoc = glGetUniformLocation(m_glowProgram, "uCam");
    glUniform1f(glowScaleLoc, camera.getScale());
    glUniform2f(glowCamLoc, static_cast<float>(camera.getX()), static_cast<float>(camera.getY()));
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(bodies.size()));

    // Renderuj solidne punkty (normal blending)
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(m_pointProgram);
    GLuint pointScaleLoc = glGetUniformLocation(m_pointProgram, "uScale");
    GLuint pointCamLoc = glGetUniformLocation(m_pointProgram, "uCam");
    glUniform1f(pointScaleLoc, camera.getScale());
    glUniform2f(pointCamLoc, static_cast<float>(camera.getX()), static_cast<float>(camera.getY()));
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(bodies.size()));

    glBindVertexArray(0);
}

void Renderer::renderAtmospheres(const Simulation& simulation, const Camera& camera) {
    const auto& bodies = simulation.getBodies();
    if (bodies.empty()) return;

    // Użyj tych samych danych co w renderPoints
    std::vector<float> pointData;
    for (const auto& body : bodies) {
        auto gpuBody = body.toGPUBody();
        pointData.push_back(gpuBody.x);
        pointData.push_back(gpuBody.y);
        pointData.push_back(gpuBody.size);
        pointData.push_back(gpuBody.r);
        pointData.push_back(gpuBody.g);
        pointData.push_back(gpuBody.b);
        pointData.push_back(gpuBody.hasAtmosphere);
        pointData.push_back(gpuBody.atmosphereSize);
        pointData.push_back(gpuBody.ar);
        pointData.push_back(gpuBody.ag);
        pointData.push_back(gpuBody.ab);
    }

    // Renderuj atmosfery
    glBindVertexArray(m_pointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_pointsVBO);
    glBufferData(GL_ARRAY_BUFFER, pointData.size() * sizeof(float), pointData.data(), GL_DYNAMIC_DRAW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(m_atmosphereProgram);
    GLuint atmScaleLoc = glGetUniformLocation(m_atmosphereProgram, "uScale");
    GLuint atmCamLoc = glGetUniformLocation(m_atmosphereProgram, "uCam");
    glUniform1f(atmScaleLoc, camera.getScale());
    glUniform2f(atmCamLoc, static_cast<float>(camera.getX()), static_cast<float>(camera.getY()));
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(bodies.size()));

    glBindVertexArray(0);
}

void Renderer::renderMoons(const Simulation& simulation, const Camera& camera) {
    const auto& bodies = simulation.getBodies();
    
    // Zbierz wszystkie księżyce do jednej listy
    std::vector<Body> allMoons;
    for (const auto& body : bodies) {
        for (const auto& moon : body.getMoons()) {
            allMoons.push_back(moon);
        }
    }
    
    if (allMoons.empty()) return;

    // Przygotuj dane księżyców
    std::vector<float> moonData;
    for (const auto& moon : allMoons) {
        auto gpuBody = moon.toGPUBody();
        moonData.push_back(gpuBody.x);
        moonData.push_back(gpuBody.y);
        moonData.push_back(gpuBody.size);
        moonData.push_back(gpuBody.r);
        moonData.push_back(gpuBody.g);
        moonData.push_back(gpuBody.b);
        moonData.push_back(0.0f); // hasAtmosphere = false
        moonData.push_back(1.0f); // atmosphereSize = 1.0
        moonData.push_back(0.0f); // atmosphere color R
        moonData.push_back(0.0f); // atmosphere color G
        moonData.push_back(0.0f); // atmosphere color B
    }

    // Renderuj księżyce
    glBindVertexArray(m_pointsVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_pointsVBO);
    glBufferData(GL_ARRAY_BUFFER, moonData.size() * sizeof(float), moonData.data(), GL_DYNAMIC_DRAW);

    // Glow dla księżyców
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glUseProgram(m_glowProgram);
    GLuint glowScaleLoc = glGetUniformLocation(m_glowProgram, "uScale");
    GLuint glowCamLoc = glGetUniformLocation(m_glowProgram, "uCam");
    glUniform1f(glowScaleLoc, camera.getScale());
    glUniform2f(glowCamLoc, static_cast<float>(camera.getX()), static_cast<float>(camera.getY()));
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(allMoons.size()));

    // Solidne księżyce
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(m_pointProgram);
    GLuint pointScaleLoc = glGetUniformLocation(m_pointProgram, "uScale");
    GLuint pointCamLoc = glGetUniformLocation(m_pointProgram, "uCam");
    glUniform1f(pointScaleLoc, camera.getScale());
    glUniform2f(pointCamLoc, static_cast<float>(camera.getX()), static_cast<float>(camera.getY()));
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(allMoons.size()));

    glBindVertexArray(0);
}

void Renderer::renderTrails(const Simulation& simulation, const Camera& camera) {
    const auto& bodies = simulation.getBodies();
    
    glUseProgram(m_lineProgram);
    GLuint lineScaleLoc = glGetUniformLocation(m_lineProgram, "uScale");
    GLuint lineCamLoc = glGetUniformLocation(m_lineProgram, "uCam");
    glUniform1f(lineScaleLoc, camera.getScale());
    glUniform2f(lineCamLoc, static_cast<float>(camera.getX()), static_cast<float>(camera.getY()));

    glBindVertexArray(m_trailVAO);

    for (const auto& body : bodies) {
        if (body.trail.size() < 4) continue;

        glBindBuffer(GL_ARRAY_BUFFER, m_trailVBO);
        glBufferData(GL_ARRAY_BUFFER, body.trail.size() * sizeof(float), body.trail.data(), GL_DYNAMIC_DRAW);

        GLuint colorLoc = glGetUniformLocation(m_lineProgram, "uColor");
        // Półprzezroczyste ślady
        glUniform3f(colorLoc, body.color[0] * 0.8f, body.color[1] * 0.8f, body.color[2] * 0.8f);
        glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(body.trail.size() / 2));
    }

    // Ślady księżyców
    for (const auto& body : bodies) {
        for (const auto& moon : body.getMoons()) {
            if (moon.trail.size() < 4) continue;

            glBindBuffer(GL_ARRAY_BUFFER, m_trailVBO);
            glBufferData(GL_ARRAY_BUFFER, moon.trail.size() * sizeof(float), moon.trail.data(), GL_DYNAMIC_DRAW);

            GLuint colorLoc = glGetUniformLocation(m_lineProgram, "uColor");
            glUniform3f(colorLoc, moon.color[0] * 0.6f, moon.color[1] * 0.6f, moon.color[2] * 0.6f);
            glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(moon.trail.size() / 2));
        }
    }

    glBindVertexArray(0);
}

void Renderer::endFrame() {
    // Optional: any post-rendering operations
}