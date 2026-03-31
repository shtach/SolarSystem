#pragma once

#include "engine/core/Vec2.h"
#include <array>
#include <vector>

class Body {
public:
    // GPU data transfer — intentionally a plain data struct
    struct GPUBody { // NOLINT(cppcoreguidelines-non-private-member-variables-in-classes)
        float x{};
        float y{};
        float size{};
        float r{};
        float g{};
        float b{};
        float hasAtmosphere{};
        float atmosphereSize{};
        float ar{};
        float ag{};
        float ab{};
    };

    Body(double mass = 0.0,
         const Vec2& position = Vec2(),
         const Vec2& velocity = Vec2(),
         float size = 1.0F);

    // --- Physics state ---
    double getMass() const { return m_mass; }

    const Vec2& getPosition() const { return m_position; }
    void setPosition(const Vec2& pos) { m_position = pos; }

    const Vec2& getVelocity() const { return m_velocity; }
    void setVelocity(const Vec2& vel) { m_velocity = vel; }

    const Vec2& getAcceleration() const { return m_acceleration; }
    void setAcceleration(const Vec2& acc) { m_acceleration = acc; }

    // --- Visual ---
    float getSize() const { return m_size; }
    const std::array<float, 3>& getColor() const { return m_color; }
    void setColor(float red, float green, float blue) {
        m_color = {red, green, blue};
    }

    // --- Trail ---
    const std::vector<float>& getTrail() const { return m_trail; }
    void updateTrail(bool enabled);
    void clearTrail() { m_trail.clear(); }

    // --- Atmosphere ---
    bool hasAtmosphere() const { return m_hasAtmosphere; }
    float getAtmosphereSize() const { return m_atmosphereSize; }
    const std::array<float, 3>& getAtmosphereColor() const { return m_atmosphereColor; }
    void setAtmosphere(float size, float red, float green, float blue);

    // --- Moons ---
    void addMoon(const Body& moon) { m_moons.push_back(moon); }
    void clearMoons() { m_moons.clear(); }
    const std::vector<Body>& getMoons() const { return m_moons; }
    std::vector<Body>& getMoons() { return m_moons; }

    // --- GPU ---
    GPUBody toGPUBody() const;

private:
    static constexpr int MAX_TRAIL_LENGTH = 1000;

    double m_mass{0.0};
    Vec2 m_position{};
    Vec2 m_velocity{};
    Vec2 m_acceleration{};
    float m_size{1.0F};
    std::array<float, 3> m_color{};

    std::vector<float> m_trail{};
    int m_trailCounter{0};

    bool m_hasAtmosphere{false};
    float m_atmosphereSize{0.0F};
    std::array<float, 3> m_atmosphereColor{0.5F, 0.5F, 1.0F};

    std::vector<Body> m_moons{};
};