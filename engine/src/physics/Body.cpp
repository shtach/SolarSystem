#include "engine/physics/Body.h"

Body::Body(double mass, const Vec2& position, const Vec2& velocity, float bodySize)
    : mass(mass), position(position), velocity(velocity), size(bodySize),
      acceleration(0.0, 0.0), color{1.0f, 1.0f, 1.0f} {
    trail.reserve(MAX_TRAIL_LENGTH * 2);
}

void Body::setAtmosphere(float size, float r, float g, float b) {
    hasAtmosphere = true;
    atmosphereSize = size;
    atmosphereColor[0] = r;
    atmosphereColor[1] = g;
    atmosphereColor[2] = b;
}

void Body::updateTrail(bool enabled) {
    if (enabled) {
        trailCounter++;

        if (trailCounter % 3 == 0) {
            trail.push_back(static_cast<float>(position.x));
            trail.push_back(static_cast<float>(position.y));

            if (trail.size() > MAX_TRAIL_LENGTH * 2) {
                trail.erase(trail.begin(), trail.begin() + 2);
            }
        }
    }
    else {
        clearTrail();
    }
}

Body::GPUBody Body::toGPUBody() const {
    return GPUBody{
        static_cast<float>(position.x),
        static_cast<float>(position.y),
        size,
        color[0], color[1], color[2],
        hasAtmosphere ? 1.0f : 0.0f,
        atmosphereSize,
        atmosphereColor[0], atmosphereColor[1], atmosphereColor[2]
    };
}