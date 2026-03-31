#include "engine/physics/Simulation.h"
#include <cmath>
#include <random>

void Simulation::initializeSolarSystem() {
    m_bodies.clear();
    m_bodies.reserve(1100);

    Body sun(1.9885e30, Vec2(0.0, 0.0), Vec2(0.0, 0.0), 50.0F);
    sun.setColor(1.0F, 0.9F, 0.1F);
    sun.setAtmosphere(1.5F, 1.0F, 0.8F, 0.2F);
    m_bodies.push_back(sun);

    auto addPlanet = [this](double mass, double radius, float size,
                             float red, float green, float blue,
                             bool atmosphere = false, float atmSize = 0.0F,
                             float ar = 0.0F, float ag = 0.0F, float ab = 0.0F) -> size_t {
        double orbitalVelocity = std::sqrt(m_G * m_bodies[0].getMass() / radius);
        Body planet(mass, Vec2(radius, 0.0), Vec2(0.0, orbitalVelocity), size);
        planet.setColor(red, green, blue);
        if (atmosphere) {
            planet.setAtmosphere(atmSize, ar, ag, ab);
        }
        m_bodies.push_back(planet);
        return m_bodies.size() - 1;
    };

    size_t mercury = addPlanet(3.3011e23, 5.791e10,  8.0F, 0.6F, 0.6F, 0.6F);
    size_t venus   = addPlanet(4.8675e24, 1.082e11, 12.0F, 0.9F, 0.7F, 0.3F, true, 1.4F, 1.0F, 0.6F, 0.2F);
    size_t earth   = addPlanet(5.972e24,  1.496e11, 13.0F, 0.2F, 0.4F, 1.0F, true, 1.3F, 0.3F, 0.5F, 1.0F);
    size_t mars    = addPlanet(6.4171e23, 2.279e11, 10.0F, 0.8F, 0.3F, 0.2F, true, 1.2F, 0.8F, 0.4F, 0.3F);
    size_t jupiter = addPlanet(1.8982e27, 7.785e11, 45.0F, 0.8F, 0.6F, 0.4F, true, 1.5F, 0.9F, 0.7F, 0.5F);
    size_t saturn  = addPlanet(5.6834e26, 1.429e12, 40.0F, 0.95F, 0.85F, 0.6F, true, 1.8F, 0.9F, 0.8F, 0.6F);
    size_t uranus  = addPlanet(8.6810e25, 2.871e12, 25.0F, 0.6F, 0.8F, 0.95F, true, 1.4F, 0.6F, 0.8F, 0.95F);
    size_t neptune = addPlanet(1.02413e26, 4.498e12, 25.0F, 0.2F, 0.2F, 0.8F, true, 1.4F, 0.2F, 0.3F, 0.9F);

    // suppress unused-variable — indices only used in addMoonsToSimulation
    (void)mercury;

    addMoonsToSimulation(earth, mars, jupiter, saturn, uranus, neptune);
    addAsteroids(1000);
}

void Simulation::addMoonsToSimulation(size_t earth, size_t mars, size_t jupiter,
                                       size_t saturn, size_t uranus, size_t neptune) {
    // Helper: add a body orbiting a parent at a given index
    auto addSatellite = [this](size_t parentIdx, double mass, double distance,
                                float size, float red, float green, float blue,
                                double angle) {
        const Vec2 parentPos = m_bodies[parentIdx].getPosition();
        const Vec2 parentVel = m_bodies[parentIdx].getVelocity();
        double orbitalVelocity = std::sqrt(m_G * m_bodies[parentIdx].getMass() / distance);

        Vec2 pos = parentPos + Vec2(distance * std::cos(angle), distance * std::sin(angle));
        Vec2 vel = parentVel + Vec2(-orbitalVelocity * std::sin(angle),
                                    orbitalVelocity * std::cos(angle));

        Body satellite(mass, pos, vel, size);
        satellite.setColor(red, green, blue);
        m_bodies.push_back(satellite);
    };

    // Earth
    addSatellite(earth,   7.3477e22, 3.844e8,  3.0F, 0.9F, 0.9F, 1.0F, 0.1);

    // Mars
    addSatellite(mars,  1.0659e16, 9.377e6,  2.0F, 0.8F, 0.7F, 0.7F, 0.2);
    addSatellite(mars,  1.4762e15, 2.346e7,  1.6F, 0.7F, 0.6F, 0.6F, 0.4);

    // Jupiter
    addSatellite(jupiter, 8.9319e22, 4.217e8,  4.5F, 1.0F, 0.9F, 0.8F, 0.0);
    addSatellite(jupiter, 4.7998e22, 6.709e8,  3.9F, 0.9F, 0.9F, 1.0F, 0.5);
    addSatellite(jupiter, 1.4819e23, 1.070e9,  6.0F, 0.8F, 0.8F, 0.9F, 1.0);
    addSatellite(jupiter, 1.0759e23, 1.883e9,  5.4F, 0.7F, 0.7F, 0.8F, 1.5);

    // Saturn
    addSatellite(saturn,  1.3452e23, 1.221e9,  5.0F, 1.0F, 0.9F, 0.8F, 0.2);
    addSatellite(saturn,  1.080e21,  1.855e8,  3.0F, 0.9F, 0.9F, 1.0F, 0.7);
    addSatellite(saturn,  1.205e21,  2.380e8,  3.25F, 0.9F, 0.9F, 1.0F, 1.2);

    // Uranus
    addSatellite(uranus,  6.59e21,   1.910e8,  3.0F, 0.8F, 0.9F, 1.0F, 0.1);
    addSatellite(uranus,  1.27e21,   2.660e8,  2.4F, 0.8F, 0.9F, 1.0F, 0.8);
    addSatellite(uranus,  3.49e21,   4.360e8,  3.2F, 0.8F, 0.9F, 1.0F, 1.4);

    // Neptune (Triton)
    addSatellite(neptune, 2.14e22,   3.547e8,  3.0F, 0.9F, 0.9F, 1.0F, 0.3);
}

void Simulation::addAsteroids(int count) {
    std::mt19937_64 rng(12345);
    std::uniform_real_distribution<double> distRadius(3.0e11, 5.0e11);
    std::uniform_real_distribution<double> distAngle(0.0, 6.283185307179586);
    std::uniform_real_distribution<double> distSize(0.3, 1.0);

    const double sunMass = m_bodies[0].getMass();

    for (int i = 0; i < count; ++i) {
        double radius = distRadius(rng);
        double angle  = distAngle(rng);
        double vel    = std::sqrt(m_G * sunMass / radius) * (0.95 + (rng() % 100) / 1000.0);

        Body asteroid(
            1e16 + static_cast<double>(rng() % 1000) * 1e13,
            Vec2(radius * std::cos(angle), radius * std::sin(angle)),
            Vec2(-std::sin(angle) * vel, std::cos(angle) * vel),
            static_cast<float>(distSize(rng))
        );
        asteroid.setColor(0.5F, 0.5F, 0.5F);
        m_bodies.push_back(asteroid);
    }
}

void Simulation::update(double frameTime, bool useCompute, bool drawTrails) {
    if (m_paused) { return; }

    m_accumulator += frameTime * m_simulationSpeed;

    constexpr double physicsStep = 86400.0;
    constexpr int maxStepsPerFrame = 1000;
    int steps = 0;

    while (m_accumulator >= physicsStep && steps < maxStepsPerFrame) {
        if (useCompute) {
            computeGravityGPU();
        } else {
            velocityVerletStep(physicsStep);
        }
        m_accumulator -= physicsStep;
        ++steps;
    }

    for (auto& body : m_bodies) {
        body.updateTrail(drawTrails);
    }
}

void Simulation::computeGravityCPU() {
    for (auto& body : m_bodies) {
        body.setAcceleration(Vec2(0.0, 0.0));
    }

    constexpr double softening = 1e10;

    for (size_t i = 0; i < m_bodies.size(); ++i) {
        for (size_t j = i + 1; j < m_bodies.size(); ++j) {
            Vec2 delta = m_bodies[j].getPosition() - m_bodies[i].getPosition();
            double distanceSq = delta.lengthSquared() + softening;
            double distance = std::sqrt(distanceSq);
            double invDistCube = 1.0 / (distanceSq * distance);
            double force = m_G * m_bodies[i].getMass() * m_bodies[j].getMass() * invDistCube;

            Vec2 forceVec = delta * force;
            m_bodies[i].setAcceleration(m_bodies[i].getAcceleration() + forceVec / m_bodies[i].getMass());
            m_bodies[j].setAcceleration(m_bodies[j].getAcceleration() - forceVec / m_bodies[j].getMass());
        }
    }
}

void Simulation::velocityVerletStep(double dt) {
    for (auto& body : m_bodies) {
        body.setPosition(
            body.getPosition() +
            body.getVelocity() * dt +
            body.getAcceleration() * (0.5 * dt * dt)
        );
    }

    std::vector<Vec2> oldAccelerations;
    oldAccelerations.reserve(m_bodies.size());
    for (const auto& body : m_bodies) {
        oldAccelerations.push_back(body.getAcceleration());
    }

    computeGravityCPU();

    for (size_t i = 0; i < m_bodies.size(); ++i) {
        m_bodies[i].setVelocity(
            m_bodies[i].getVelocity() +
            (oldAccelerations[i] + m_bodies[i].getAcceleration()) * (0.5 * dt)
        );
    }
}

void Simulation::computeGravityGPU() {
    computeGravityCPU();
}