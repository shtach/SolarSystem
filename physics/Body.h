#pragma once
#include "Vec2.h"
#include <vector>

class Body {
private:
    static constexpr int MAX_TRAIL_LENGTH = 1000; // ZWIĘKSZONE: dłuższe ślady

public:
    double mass;
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;
    float size;
    float color[3];
    std::vector<float> trail;
    
    bool hasAtmosphere = false;
    float atmosphereSize = 0.0f;
    float atmosphereColor[3] = {0.5f, 0.5f, 1.0f};
    std::vector<Body> moons;

    Body(double mass = 0.0, const Vec2& position = Vec2(),
        const Vec2& velocity = Vec2(), float size = 1.0f);

    void updateTrail(bool enabled);
    void clearTrail() { trail.clear(); }
    
    void setAtmosphere(float size, float r, float g, float b);
    
    void addMoon(const Body& moon) { moons.push_back(moon); }
    void clearMoons() { moons.clear(); }
    const std::vector<Body>& getMoons() const { return moons; }

    struct GPUBody {
        float x, y;
        float size;
        float r, g, b;
        float hasAtmosphere;
        float atmosphereSize;
        float ar, ag, ab;
    };

    GPUBody toGPUBody() const;
};