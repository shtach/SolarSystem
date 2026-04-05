#include "engine/physics/Body.h"

Body::Body(double mass, const Vec2& position, const Vec2& velocity, float bodySize)
    : m_mass(mass)
    , m_position(position)
    , m_velocity(velocity)
    , m_acceleration(0.0, 0.0)
    , m_size(bodySize)
    , m_color{1.0F, 1.0F, 1.0F}
{
    m_trail.reserve(MAX_TRAIL_LENGTH * 2);
}

void Body::setAtmosphere(float size, float red, float green, float blue) {
    m_hasAtmosphere = true;
    m_atmosphereSize = size;
    m_atmosphereColor[0] = red;
    m_atmosphereColor[1] = green;
    m_atmosphereColor[2] = blue;
}

void Body::updateTrail(bool enabled) {
    if (!enabled) {
        clearTrail();
        return;
    }

    ++m_trailCounter;
    if (m_trailCounter % 3 != 0) {
        return;
    }

    m_trail.push_back(static_cast<float>(m_position.x));
    m_trail.push_back(static_cast<float>(m_position.y));

    if (m_trail.size() > static_cast<size_t>(MAX_TRAIL_LENGTH * 2)) {
        m_trail.erase(m_trail.begin(), m_trail.begin() + 2);
    }
}

Body::GPUBody Body::toGPUBody() const {
    return GPUBody{
        static_cast<float>(m_position.x),
        static_cast<float>(m_position.y),
        m_size,
        m_color[0], m_color[1], m_color[2],
        m_hasAtmosphere ? 1.0F : 0.0F,
        m_atmosphereSize,
        m_atmosphereColor[0], m_atmosphereColor[1], m_atmosphereColor[2],
        m_texIndex
    };
}