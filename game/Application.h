#pragma once

#include "Renderer.h"
#include "Simulation.h"
#include "Camera.h"
#include "InputHandler.h"

class Application {
private:
    GLFWwindow* m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Simulation> m_simulation;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<InputHandler> m_inputHandler;

    bool m_paused = false;
    double m_lastTime = 0.0;

    void initializeGLFW();
    void initializeGLAD();
    void initializeSystems();
    void processInput(double frameTime); // DODANO PARAMETR
    void update(double frameTime);
    void render();

public:
    Application();
    ~Application();

    void run();
};