#include "Application.h"
#include <iostream>

Application::Application() {
    initializeGLFW();
    initializeGLAD();
    initializeSystems();
}

Application::~Application() {
    glfwTerminate();
}

void Application::initializeGLFW() {
    if (!glfwInit()) {
        throw std::runtime_error("GLFW initialization failed");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(1600, 900, "Solar System Simulation - Complete", nullptr, nullptr);
    if (!m_window) {
        throw std::runtime_error("Window creation failed");
    }

    glfwMakeContextCurrent(m_window);
}

void Application::initializeGLAD() {
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        throw std::runtime_error("GLAD initialization failed");
    }
}

void Application::initializeSystems() {
    m_camera     = std::make_unique<Camera>();
    m_renderer   = std::make_unique<Renderer>();
    m_simulation = std::make_unique<Simulation>();
    m_inputHandler = std::make_unique<InputHandler>(m_window, m_camera.get());

    m_simulation->initializeSolarSystem();
    m_lastTime = glfwGetTime();
    m_camera->viewWholeSystem();
}

void Application::processInput(double frameTime) {
    m_inputHandler->processInput(m_paused);

    if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) { m_camera->move( 0,  1, frameTime); }
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) { m_camera->move( 0, -1, frameTime); }
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) { m_camera->move(-1,  0, frameTime); }
    if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) { m_camera->move( 1,  0, frameTime); }

    if (glfwGetKey(m_window, GLFW_KEY_R) == GLFW_PRESS) {
        m_camera->viewWholeSystem();
        glfwWaitEventsTimeout(0.1);
    }

    if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS) {
        m_simulation->setSimulationSpeed(
            std::min(m_simulation->getSimulationSpeed() * 2.0, 1e9));
        std::cout << "Simulation speed: " << m_simulation->getSimulationSpeed() << "x\n";
        glfwWaitEventsTimeout(0.1);
    }
    if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        m_simulation->setSimulationSpeed(
            std::max(m_simulation->getSimulationSpeed() / 2.0, 1e4));
        std::cout << "Simulation speed: " << m_simulation->getSimulationSpeed() << "x\n";
        glfwWaitEventsTimeout(0.1);
    }
    if (glfwGetKey(m_window, GLFW_KEY_1) == GLFW_PRESS) {
        m_simulation->setSimulationSpeed(1e6);
        std::cout << "Simulation speed: 1e6x\n";
        glfwWaitEventsTimeout(0.1);
    }
}

void Application::update(double frameTime) {
    m_simulation->update(frameTime, m_inputHandler->shouldUseCompute(),
        m_inputHandler->shouldDrawTrails());
}

void Application::render() {
    m_renderer->beginFrame();
    m_renderer->render(*m_simulation, *m_camera);
    m_renderer->endFrame();
}

void Application::run() {
    std::cout << "=== SOLAR SYSTEM SIMULATION ===\n"
              << "WASD - move | Scroll - zoom | Drag - pan\n"
              << "Space - pause | R - reset view\n"
              << "UP/DOWN - speed | 1 - reset speed\n"
              << "Speed: " << m_simulation->getSimulationSpeed() << "x | "
              << "Bodies: " << m_simulation->getBodyCount() << "\n";

    while (!glfwWindowShouldClose(m_window)) {
        double currentTime = glfwGetTime();
        double frameTime   = currentTime - m_lastTime;
        m_lastTime         = currentTime;

        processInput(frameTime);
        if (!m_paused) { update(frameTime); }
        render();

        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}