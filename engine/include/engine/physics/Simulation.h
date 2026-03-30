#pragma once
#include "Body.h"
#include <vector>

class Simulation {
private:
    std::vector<Body> m_bodies;
    double m_accumulator = 0.0;
    const double m_physicsDt = 86400.0; // ZMIENIONE: 1 dzień w sekundach
    const double m_G = 6.67430e-11;
    bool m_paused = false;
    double m_simulationSpeed = 1.0;

    void computeGravityCPU();
    void velocityVerletStep(double dt);
    void computeGravityGPU();
    void addMoonsToSimulation(size_t earth, size_t mars, size_t jupiter, size_t saturn, size_t uranus, size_t neptune);
    void addAsteroids(int count);

public:
    Simulation() = default;

    void initializeSolarSystem();
    void update(double frameTime, bool useCompute, bool drawTrails);

    const std::vector<Body>& getBodies() const { return m_bodies; }
    size_t getBodyCount() const { return m_bodies.size(); }

    void addBody(const Body& body) { m_bodies.push_back(body); }
    void clearBodies() { m_bodies.clear(); }
    
    void setPaused(bool paused) { m_paused = paused; }
    bool isPaused() const { return m_paused; }
    
    void setSimulationSpeed(double speed) { m_simulationSpeed = speed; }
    double getSimulationSpeed() const { return m_simulationSpeed; }
};