#pragma once

class Camera {
public:
    Camera() = default;

    void move(double dx, double dy, double deltaTime);
    void zoom(float factor);
    void pan(double dx, double dy, int windowWidth, int windowHeight);
    void viewWholeSystem();

    double getX()     const { return m_x; }
    double getY()     const { return m_y; }
    float  getScale() const { return m_scale; }

private:
    double m_x     = 0.0;
    double m_y     = 0.0;
    float  m_scale = 2e-13F;

    static constexpr float MIN_SCALE = 1e-14F;
    static constexpr float MAX_SCALE = 1e-9F;
    static constexpr float DEFAULT_SCALE = 2e-13F;
};