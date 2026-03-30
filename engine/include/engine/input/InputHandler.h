#pragma once
#include "engine/graphics/Camera.h"
#include <GLFW/glfw3.h>
#include <map>

class InputHandler {
private:
    GLFWwindow* m_window;
    Camera* m_camera;

    bool m_dragging = false;
    double m_lastMouseX = 0.0, m_lastMouseY = 0.0;

    bool m_useCompute = false;
    bool m_drawTrails = true;

    static std::map<GLFWwindow*, InputHandler*> s_instances;

    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

    static InputHandler* getInstance(GLFWwindow* window);

public:
    InputHandler(GLFWwindow* window, Camera* camera);
    ~InputHandler();

    void processInput(bool& paused);

    bool shouldUseCompute() const { return m_useCompute; }
    bool shouldDrawTrails() const { return m_drawTrails; }

    void handleScroll(double yoffset);
    void handleMouseButton(int button, int action);
    void handleCursorPos(double x, double y);
};