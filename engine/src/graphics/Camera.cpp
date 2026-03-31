#include "engine/graphics/Camera.h"

void Camera::move(double dx, double dy, double deltaTime) {
    double speed = (1.0 / static_cast<double>(m_scale)) * deltaTime * 0.1;
    m_x += dx * speed;
    m_y += dy * speed;
}

void Camera::zoom(float factor) {
    m_scale *= (factor > 0.0F) ? 1.1F : 0.9F;
    if (m_scale < MIN_SCALE) { m_scale = MIN_SCALE; }
    if (m_scale > MAX_SCALE) { m_scale = MAX_SCALE; }
}

void Camera::pan(double dx, double dy, int windowWidth, int windowHeight) {
    m_x -= dx * 2.0 / (static_cast<double>(m_scale) * windowWidth);
    m_y += dy * 2.0 / (static_cast<double>(m_scale) * windowHeight);
}

void Camera::viewWholeSystem() {
    m_x     = 0.0;
    m_y     = 0.0;
    m_scale = DEFAULT_SCALE;
}