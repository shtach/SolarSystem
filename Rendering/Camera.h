#pragma once

class Camera {
private:
    double m_x = 0.0, m_y = 0.0;
    float m_scale = 1e-11f; // ZMIENIONE: lepsze początkowe przybliżenie

public:
    Camera() = default;

    void move(double dx, double dy, double deltaTime) {
        double speed = 1.0 / m_scale * deltaTime * 0.1;
        m_x += dx * speed;
        m_y += dy * speed;
    }

    void zoom(double factor) {
        m_scale *= (factor > 0) ? 1.1f : 0.9f;
        
        // Szerszy zakres zoom
        if (m_scale < 1e-14f) m_scale = 1e-14f;
        if (m_scale > 1e-8f) m_scale = 1e-8f; // ZWIĘKSZONY maksymalny zoom
    }

    void pan(double dx, double dy, int windowWidth, int windowHeight) {
        m_x -= dx * 2.0 / (m_scale * windowWidth);
        m_y += dy * 2.0 / (m_scale * windowHeight);
    }

    double getX() const { return m_x; }
    double getY() const { return m_y; }
    float getScale() const { return m_scale; }
    
    void viewWholeSystem() {
        m_x = 0.0;
        m_y = 0.0;
        m_scale = 1e-11f; // ZMIENIONE: lepszy widok całego układu
    }
};