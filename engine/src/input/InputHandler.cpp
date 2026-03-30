#include "engine/input/InputHandler.h"
#include <thread>   
#include <chrono>
#include <iostream>

std::map<GLFWwindow*, InputHandler*> InputHandler::s_instances;

InputHandler::InputHandler(GLFWwindow* window, Camera* camera)
    : m_window(window), m_camera(camera) {
    s_instances[window] = this;
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
}

InputHandler::~InputHandler() {
    s_instances.erase(m_window);
}

InputHandler* InputHandler::getInstance(GLFWwindow* window) {
    auto it = s_instances.find(window);
    return (it != s_instances.end()) ? it->second : nullptr;
}

void InputHandler::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    (void)xoffset; // Wycisz warning
    InputHandler* instance = getInstance(window);
    if (instance) {
        instance->handleScroll(yoffset);
    }
}

void InputHandler::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    (void)mods; // Wycisz warning
    InputHandler* instance = getInstance(window);
    if (instance) {
        instance->handleMouseButton(button, action);
    }
}

void InputHandler::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    InputHandler* instance = getInstance(window);
    if (instance) {
        instance->handleCursorPos(xpos, ypos);
    }
}

void InputHandler::processInput(bool& paused) {
    // Toggle pause
    if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        paused = !paused;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        std::cout << "Simulation: " << (paused ? "PAUSED" : "RUNNING") << std::endl;
    }

    // Toggle compute shader
    if (glfwGetKey(m_window, GLFW_KEY_C) == GLFW_PRESS) {
        m_useCompute = !m_useCompute;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        std::cout << "Compute shader: " << (m_useCompute ? "ON" : "OFF") << std::endl;
    }

    // Toggle trails
    if (glfwGetKey(m_window, GLFW_KEY_T) == GLFW_PRESS) {
        m_drawTrails = !m_drawTrails;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
        std::cout << "Trails: " << (m_drawTrails ? "ON" : "OFF") << std::endl;
    }
}

void InputHandler::handleScroll(double yoffset) {
    m_camera->zoom(yoffset);
}

void InputHandler::handleMouseButton(int button, int action) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        m_dragging = (action == GLFW_PRESS);
        if (m_dragging) {
            glfwGetCursorPos(m_window, &m_lastMouseX, &m_lastMouseY);
        }
    }
}

void InputHandler::handleCursorPos(double x, double y) {
    if (m_dragging) {
        int width, height;
        glfwGetWindowSize(m_window, &width, &height);
        m_camera->pan(x - m_lastMouseX, y - m_lastMouseY, width, height);
        m_lastMouseX = x;
        m_lastMouseY = y;
    }
}