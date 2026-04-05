#include "engine/physics/Simulation.h"
#include "engine/physics/QuadTree.h"

#include <cmath>
#include <random>

void Simulation::initializeSolarSystem() {
    m_bodies.clear();
    m_bodies.reserve(1100);

    Body sun(1.9885e30, Vec2(0.0, 0.0), Vec2(0.0, 0.0), 55.0F);
    sun.setColor(1.0F, 0.92F, 0.15F);
    sun.setAtmosphere(1.6F, 1.0F, 0.75F, 0.1F);
    sun.setTexIndex(0.0F);
    m_bodies.push_back(sun);

    auto addPlanet = [this](double mass, double radius, float size,
                             float red, float green, float blue,
                             float texIdx,
                             bool atmosphere = false, float atmSize = 0.0F,
                             float ar = 0.0F, float ag = 0.0F, float ab = 0.0F) -> size_t {
        double orbitalVelocity = std::sqrt(m_G * m_bodies[0].getMass() / radius);
        Body planet(mass, Vec2(radius, 0.0), Vec2(0.0, orbitalVelocity), size);
        planet.setColor(red, green, blue);
        planet.setTexIndex(texIdx);
        if (atmosphere) {
            planet.setAtmosphere(atmSize, ar, ag, ab);
        }
        m_bodies.push_back(planet);
        return m_bodies.size() - 1;
    };

    // Sizes scaled for visibility — larger values = more pixels for shading detail
    // texIndex: 1=mercury 2=venus 3=earth 4=mars 5=jupiter 6=saturn 7=uranus 8=neptune
    size_t mercury = addPlanet(3.3011e23, 5.791e10, 13.0F, 0.68F, 0.58F, 0.50F, 1.0F);
    size_t venus   = addPlanet(4.8675e24, 1.082e11, 18.0F, 0.98F, 0.88F, 0.60F, 2.0F, true, 1.5F, 1.0F, 0.80F, 0.35F);
    size_t earth   = addPlanet(5.972e24,  1.496e11, 20.0F, 0.20F, 0.52F, 1.00F, 3.0F, true, 1.4F, 0.3F, 0.60F, 1.00F);
    size_t mars    = addPlanet(6.4171e23, 2.279e11, 15.0F, 0.92F, 0.32F, 0.12F, 4.0F, true, 1.3F, 0.9F, 0.45F, 0.25F);
    size_t jupiter = addPlanet(1.8982e27, 7.785e11, 58.0F, 0.85F, 0.65F, 0.42F, 5.0F, true, 1.6F, 0.95F, 0.75F, 0.50F);
    size_t saturn  = addPlanet(5.6834e26, 1.429e12, 50.0F, 0.96F, 0.88F, 0.62F, 6.0F, true, 1.9F, 0.96F, 0.85F, 0.60F);
    size_t uranus  = addPlanet(8.6810e25, 2.871e12, 35.0F, 0.52F, 0.86F, 0.95F, 7.0F, true, 1.5F, 0.52F, 0.85F, 0.98F);
    size_t neptune = addPlanet(1.02413e26, 4.498e12, 35.0F, 0.15F, 0.22F, 0.98F, 8.0F, true, 1.5F, 0.18F, 0.32F, 1.00F);

    // suppress unused-variable — indices only used in addMoonsToSimulation
    (void)mercury;

    addMoonsToSimulation(earth, mars, jupiter, saturn, uranus, neptune);
    addAsteroids(1000);
}

void Simulation::addMoonsToSimulation(size_t earth, size_t mars, size_t jupiter,
                                       size_t saturn, size_t uranus, size_t neptune) {
    // Helper: add a body orbiting a parent at a given index
    // texIndex: 9=moon_rocky  10=moon_icy  11=moon_volcanic
    auto addSatellite = [this](size_t parentIdx, double mass, double distance,
                                float size, float red, float green, float blue,
                                double angle, float texIdx = 9.0F) {
        const Vec2 parentPos = m_bodies[parentIdx].getPosition();
        const Vec2 parentVel = m_bodies[parentIdx].getVelocity();
        double orbitalVelocity = std::sqrt(m_G * m_bodies[parentIdx].getMass() / distance);

        Vec2 pos = parentPos + Vec2(distance * std::cos(angle), distance * std::sin(angle));
        Vec2 vel = parentVel + Vec2(-orbitalVelocity * std::sin(angle),
                                    orbitalVelocity * std::cos(angle));

        Body satellite(mass, pos, vel, size);
        satellite.setColor(red, green, blue);
        satellite.setTexIndex(texIdx);
        m_bodies.push_back(satellite);
    };

    // Earth — Moon (rocky)
    addSatellite(earth,   7.3477e22, 3.844e8,  3.0F, 0.9F, 0.9F, 1.0F, 0.1, 9.0F);

    // Mars — Phobos, Deimos (rocky)
    addSatellite(mars,  1.0659e16, 9.377e6,  2.0F, 0.8F, 0.7F, 0.7F, 0.2, 9.0F);
    addSatellite(mars,  1.4762e15, 2.346e7,  1.6F, 0.7F, 0.6F, 0.6F, 0.4, 9.0F);

    // Jupiter — Io(volcanic), Europa(icy), Ganymede(icy), Callisto(rocky)
    addSatellite(jupiter, 8.9319e22, 4.217e8,  4.5F, 1.0F, 0.9F, 0.8F, 0.0, 11.0F);
    addSatellite(jupiter, 4.7998e22, 6.709e8,  3.9F, 0.9F, 0.9F, 1.0F, 0.5, 10.0F);
    addSatellite(jupiter, 1.4819e23, 1.070e9,  6.0F, 0.8F, 0.8F, 0.9F, 1.0, 10.0F);
    addSatellite(jupiter, 1.0759e23, 1.883e9,  5.4F, 0.7F, 0.7F, 0.8F, 1.5, 9.0F);

    // Saturn — Titan(volcanic/hazy), Enceladus(icy), Tethys(icy)
    addSatellite(saturn,  1.3452e23, 1.221e9,  5.0F, 1.0F, 0.9F, 0.8F, 0.2, 11.0F);
    addSatellite(saturn,  1.080e21,  1.855e8,  3.0F, 0.9F, 0.9F, 1.0F, 0.7, 10.0F);
    addSatellite(saturn,  1.205e21,  2.380e8,  3.25F, 0.9F, 0.9F, 1.0F, 1.2, 10.0F);

    // Uranus moons (rocky)
    addSatellite(uranus,  6.59e21,   1.910e8,  3.0F, 0.8F, 0.9F, 1.0F, 0.1, 9.0F);
    addSatellite(uranus,  1.27e21,   2.660e8,  2.4F, 0.8F, 0.9F, 1.0F, 0.8, 9.0F);
    addSatellite(uranus,  3.49e21,   4.360e8,  3.2F, 0.8F, 0.9F, 1.0F, 1.4, 9.0F);

    // Neptune — Triton (icy)
    addSatellite(neptune, 2.14e22,   3.547e8,  3.0F, 0.9F, 0.9F, 1.0F, 0.3, 10.0F);
}

void Simulation::addAsteroids(int count) {
    std::mt19937_64 rng(12345);
    std::uniform_real_distribution<double> distRadius(3.3e11, 4.8e11);
    std::uniform_real_distribution<double> distAngle(0.0, 6.283185307179586);
    std::uniform_real_distribution<double> distSize(0.3, 1.2);
    std::uniform_real_distribution<double> distEcc(-0.08, 0.08);
    std::uniform_real_distribution<double> distType(0.0, 1.0);

    const double sunMass = m_bodies[0].getMass();

    for (int i = 0; i < count; ++i) {
        double radius   = distRadius(rng);
        double angle    = distAngle(rng);
        double vel      = std::sqrt(m_G * sunMass / radius) * (1.0 + distEcc(rng));
        double typeRoll = distType(rng);

        float r{}, g{}, b{}, sz{};

        if (typeRoll < 0.75) {
            // C-type: dark carbonaceous (~75%)
            float v = 0.13F + static_cast<float>(rng() % 100) / 800.0F;
            r = v + 0.02F; g = v; b = v * 0.85F;
            sz = static_cast<float>(distSize(rng)) * 0.8F;
        } else if (typeRoll < 0.92) {
            // S-type: stony, brownish-grey (~17%)
            float base = 0.42F + static_cast<float>(rng() % 100) / 450.0F;
            r = base + 0.12F; g = base * 0.82F; b = base * 0.62F;
            sz = static_cast<float>(distSize(rng)) * 1.1F;
        } else {
            // M-type: metallic, brighter grey (~8%)
            float v = 0.58F + static_cast<float>(rng() % 100) / 550.0F;
            r = v; g = v * 0.96F; b = v * 0.90F;
            sz = static_cast<float>(distSize(rng)) * 1.3F;
        }

        Body asteroid(
            1e16 + static_cast<double>(rng() % 1000) * 1e13,
            Vec2(radius * std::cos(angle), radius * std::sin(angle)),
            Vec2(-std::sin(angle) * vel,    std::cos(angle) * vel),
            sz
        );
        asteroid.setColor(r, g, b);
        m_bodies.push_back(asteroid);
    }
}

void Simulation::update(double frameTime, bool useCompute, bool drawTrails) {
    if (m_paused) return;

    m_accumulator += frameTime * m_simulationSpeed;

    constexpr double physicsStep = 3600.0;
    constexpr int maxStepsPerFrame = 5;

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
    // Find bounding box — tree needs to contain all bodies
    double maxCoord = 0.0;
    for (const auto& body : m_bodies) {
        maxCoord = std::max(maxCoord, std::abs(body.getPosition().x));
        maxCoord = std::max(maxCoord, std::abs(body.getPosition().y));
    }
    const double halfSize = maxCoord * 1.1; // 10% margin

    QuadTree tree(Vec2(0.0, 0.0), halfSize);
    for (int i = 0; i < static_cast<int>(m_bodies.size()); ++i) {
        tree.insert(i, m_bodies[i].getPosition(), m_bodies[i].getMass());
    }

    constexpr double theta     = 0.5;
    constexpr double softening = 1e10;

    for (int i = 0; i < static_cast<int>(m_bodies.size()); ++i) {
        Vec2 acc{0.0, 0.0};
        tree.computeForce(i, m_bodies[i].getPosition(),
                          acc, m_G, theta, softening);
        m_bodies[i].setAcceleration(acc);
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