#include "Simulation.h"
#include <random>
#include <cmath>
#include <iostream>
#include <algorithm>

void Simulation::initializeSolarSystem() {
    m_bodies.clear();
    m_bodies.reserve(1100);

    // Sun
    Body sun(1.9885e30, Vec2(0.0, 0.0), Vec2(0.0, 0.0), 50.0f);
    sun.color[0] = 1.0f; sun.color[1] = 0.9f; sun.color[2] = 0.1f;
    sun.setAtmosphere(1.5f, 1.0f, 0.8f, 0.2f);
    m_bodies.push_back(sun);

    auto addPlanet = [this, &sun](double mass, double radius, float size, float r, float g, float b,
                                bool hasAtmosphere = false, float atmSize = 0.0f,
                                float ar = 0.0f, float ag = 0.0f, float ab = 0.0f) -> size_t {
        double orbital_velocity = std::sqrt(m_G * sun.mass / radius);
        Body planet(mass, Vec2(radius, 0.0), Vec2(0.0, orbital_velocity), size);
        planet.color[0] = r; planet.color[1] = g; planet.color[2] = b;
        if (hasAtmosphere) {
            planet.setAtmosphere(atmSize, ar, ag, ab);
        }
        m_bodies.push_back(planet);
        return m_bodies.size() - 1;
    };

    // Planety - POPRAWIONE: wszystkie planety dodawane do wektora ciał
    size_t mercury = addPlanet(3.3011e23, 5.791e10, 8.0f, 0.6f, 0.6f, 0.6f);
    size_t venus = addPlanet(4.8675e24, 1.082e11, 12.0f, 0.9f, 0.7f, 0.3f, true, 1.4f, 1.0f, 0.6f, 0.2f);
    size_t earth = addPlanet(5.972e24, 1.496e11, 13.0f, 0.2f, 0.4f, 1.0f, true, 1.3f, 0.3f, 0.5f, 1.0f);
    size_t mars = addPlanet(6.4171e23, 2.279e11, 10.0f, 0.8f, 0.3f, 0.2f, true, 1.2f, 0.8f, 0.4f, 0.3f);
    size_t jupiter = addPlanet(1.8982e27, 7.785e11, 45.0f, 0.8f, 0.6f, 0.4f, true, 1.5f, 0.9f, 0.7f, 0.5f);
    size_t saturn = addPlanet(5.6834e26, 1.429e12, 40.0f, 0.95f, 0.85f, 0.6f, true, 1.8f, 0.9f, 0.8f, 0.6f);
    size_t uranus = addPlanet(8.6810e25, 2.871e12, 25.0f, 0.6f, 0.8f, 0.95f, true, 1.4f, 0.6f, 0.8f, 0.95f);
    size_t neptune = addPlanet(1.02413e26, 4.498e12, 25.0f, 0.2f, 0.2f, 0.8f, true, 1.4f, 0.2f, 0.3f, 0.9f);

    // DODANE: księżyce dodawane jako osobne ciała w symulacji
    addMoonsToSimulation(earth, mars, jupiter, saturn, uranus, neptune);
    addAsteroids(1000);
}

// NOWA METODA: dodawanie księżyców jako osobnych ciał w symulacji
void Simulation::addMoonsToSimulation(Body& earth, Body& mars, Body& jupiter, Body& saturn, Body& uranus, Body& neptune) {
    // Earth's Moon
    double moonDistance = 3.844e8;
    double moonAngle = 0.1;
    Vec2 moonPos = earth.position + Vec2(moonDistance * std::cos(moonAngle), moonDistance * std::sin(moonAngle));
    double moonOrbitalVelocity = std::sqrt(m_G * earth.mass / moonDistance);
    Vec2 moonVel = earth.velocity + Vec2(-moonOrbitalVelocity * std::sin(moonAngle), moonOrbitalVelocity * std::cos(moonAngle));
    
    Body moon(7.3477e22, moonPos, moonVel, 3.0f); // ZWIĘKSZONY ROZMIAR
    moon.color[0] = 0.9f; moon.color[1] = 0.9f; moon.color[2] = 1.0f; // JAŚNIEJSZY KOLOR
    m_bodies.push_back(moon);

    // Mars moons
    auto addMarsMoon = [&](double mass, double distance, float size, float r, float g, float b, double angle) {
        Vec2 pos = mars.position + Vec2(distance * std::cos(angle), distance * std::sin(angle));
        double orbitalVelocity = std::sqrt(m_G * mars.mass / distance);
        Vec2 vel = mars.velocity + Vec2(-orbitalVelocity * std::sin(angle), orbitalVelocity * std::cos(angle));
        
        Body moonMars(mass, pos, vel, size * 2.0f); // ZWIĘKSZONY ROZMIAR
        moonMars.color[0] = r; moonMars.color[1] = g; moonMars.color[2] = b;
        m_bodies.push_back(moonMars);
    };

    addMarsMoon(1.0659e16, 9.377e6, 1.0f, 0.8f, 0.7f, 0.7f, 0.2);
    addMarsMoon(1.4762e15, 2.346e7, 0.8f, 0.7f, 0.6f, 0.6f, 0.4);

    // Jupiter moons (Galilean moons) - ZWIĘKSZONE ROZMIARY
    auto addJupiterMoon = [&](double mass, double distance, float size, float r, float g, float b, double angle) {
        Vec2 pos = jupiter.position + Vec2(distance * std::cos(angle), distance * std::sin(angle));
        double orbitalVelocity = std::sqrt(m_G * jupiter.mass / distance);
        Vec2 vel = jupiter.velocity + Vec2(-orbitalVelocity * std::sin(angle), orbitalVelocity * std::cos(angle));
        
        Body moonJ(mass, pos, vel, size * 3.0f); // ZWIĘKSZONY ROZMIAR
        moonJ.color[0] = r; moonJ.color[1] = g; moonJ.color[2] = b;
        m_bodies.push_back(moonJ);
    };

    addJupiterMoon(8.9319e22, 4.217e8, 1.5f, 1.0f, 0.9f, 0.8f, 0.0);
    addJupiterMoon(4.7998e22, 6.709e8, 1.3f, 0.9f, 0.9f, 1.0f, 0.5);
    addJupiterMoon(1.4819e23, 1.070e9, 2.0f, 0.8f, 0.8f, 0.9f, 1.0);
    addJupiterMoon(1.0759e23, 1.883e9, 1.8f, 0.7f, 0.7f, 0.8f, 1.5);

    // Saturn moons - ZWIĘKSZONE ROZMIARY
    auto addSaturnMoon = [&](double mass, double distance, float size, float r, float g, float b, double angle) {
        Vec2 pos = saturn.position + Vec2(distance * std::cos(angle), distance * std::sin(angle));
        double orbitalVelocity = std::sqrt(m_G * saturn.mass / distance);
        Vec2 vel = saturn.velocity + Vec2(-orbitalVelocity * std::sin(angle), orbitalVelocity * std::cos(angle));
        
        Body moonS(mass, pos, vel, size * 2.5f); // ZWIĘKSZONY ROZMIAR
        moonS.color[0] = r; moonS.color[1] = g; moonS.color[2] = b;
        m_bodies.push_back(moonS);
    };

    addSaturnMoon(1.3452e23, 1.221e9, 2.0f, 1.0f, 0.9f, 0.8f, 0.2);
    addSaturnMoon(1.080e21, 1.855e8, 1.2f, 0.9f, 0.9f, 1.0f, 0.7);
    addSaturnMoon(1.205e21, 2.380e8, 1.3f, 0.9f, 0.9f, 1.0f, 1.2);

    // Uranus moons - ZWIĘKSZONE ROZMIARY
    auto addUranusMoon = [&](double mass, double distance, float size, float r, float g, float b, double angle) {
        Vec2 pos = uranus.position + Vec2(distance * std::cos(angle), distance * std::sin(angle));
        double orbitalVelocity = std::sqrt(m_G * uranus.mass / distance);
        Vec2 vel = uranus.velocity + Vec2(-orbitalVelocity * std::sin(angle), orbitalVelocity * std::cos(angle));
        
        Body moonU(mass, pos, vel, size * 2.0f); // ZWIĘKSZONY ROZMIAR
        moonU.color[0] = r; moonU.color[1] = g; moonU.color[2] = b;
        m_bodies.push_back(moonU);
    };

    addUranusMoon(6.59e21, 1.910e8, 1.5f, 0.8f, 0.9f, 1.0f, 0.1);
    addUranusMoon(1.27e21, 2.660e8, 1.2f, 0.8f, 0.9f, 1.0f, 0.8);
    addUranusMoon(3.49e21, 4.360e8, 1.6f, 0.8f, 0.9f, 1.0f, 1.4);

    // Neptune moon - Triton - ZWIĘKSZONY ROZMIAR
    double tritonAngle = 0.3;
    Vec2 tritonPos = neptune.position + Vec2(3.547e8 * std::cos(tritonAngle), 3.547e8 * std::sin(tritonAngle));
    double tritonVelocity = std::sqrt(m_G * neptune.mass / 3.547e8);
    Vec2 tritonVel = neptune.velocity + Vec2(-tritonVelocity * std::sin(tritonAngle), tritonVelocity * std::cos(tritonAngle));
    
    Body triton(2.14e22, tritonPos, tritonVel, 3.0f); // ZWIĘKSZONY ROZMIAR
    triton.color[0] = 0.9f; triton.color[1] = 0.9f; triton.color[2] = 1.0f;
    m_bodies.push_back(triton);
}




void Simulation::addAsteroids(int count) {
    std::mt19937_64 rng(12345);
    std::uniform_real_distribution<double> distR(3.0e11, 5.0e11);
    std::uniform_real_distribution<double> distAngle(0.0, 6.283185307179586);
    std::uniform_real_distribution<double> distSize(0.3, 1.0);
    
    const Body& sun = m_bodies[0];

    for (int i = 0; i < count; i++) {
        double r = distR(rng);
        double angle = distAngle(rng);
        double x = r * std::cos(angle);
        double y = r * std::sin(angle);
        double asteroid_velocity = std::sqrt(m_G * sun.mass / r) * (0.95 + (rng() % 100) / 1000.0);

        // Użyj static_cast do float, aby uniknąć ostrzeżenia
        float asteroidSize = static_cast<float>(distSize(rng));
        Body asteroid(1e16 + (rng() % 1000) * 1e13,
                     Vec2(x, y),
                     Vec2(-std::sin(angle) * asteroid_velocity, std::cos(angle) * asteroid_velocity),
                     asteroidSize);
        asteroid.color[0] = 0.5f; 
        asteroid.color[1] = 0.5f; 
        asteroid.color[2] = 0.5f;
        m_bodies.push_back(asteroid);
    }
}

void Simulation::update(double frameTime, bool useCompute, bool drawTrails) {
    if (m_paused) return;

    m_accumulator += frameTime * m_simulationSpeed;

    // ZMIENIONE: dłuższy krok czasowy dla szybszego ruchu
    const double physicsStep = 86400.0; // 1 dzień zamiast 1 godziny
    int steps = 0;
    const int maxStepsPerFrame = 1000; // ZWIĘKSZONE: więcej kroków na klatkę
    
    while (m_accumulator >= physicsStep && steps < maxStepsPerFrame) {
        if (useCompute) {
            computeGravityGPU();
        }
        else {
            velocityVerletStep(physicsStep);
        }
        m_accumulator -= physicsStep;
        steps++;
    }

    // Aktualizuj ślady
    for (auto& body : m_bodies) {
        body.updateTrail(drawTrails);
    }
}

void Simulation::computeGravityCPU() {
    // Reset accelerations
    for (auto& body : m_bodies) {
        body.acceleration = Vec2(0.0, 0.0);
    }

    // ZMIENIONE: znacznie mniejsze softening dla silniejszej grawitacji
    const double softening = 1e10; // Zwiększone dla stabilności, ale mniejsze niż wcześniej

    for (size_t i = 0; i < m_bodies.size(); i++) {
        for (size_t j = i + 1; j < m_bodies.size(); j++) {
            Vec2 delta = m_bodies[j].position - m_bodies[i].position;
            double distanceSq = delta.lengthSquared();
            
            // Dodaj softening
            distanceSq += softening;
            
            double distance = std::sqrt(distanceSq);
            double invDistCube = 1.0 / (distanceSq * distance);
            double force = m_G * m_bodies[i].mass * m_bodies[j].mass * invDistCube;

            Vec2 forceVec = delta * force;
            
            m_bodies[i].acceleration += forceVec / m_bodies[i].mass;
            m_bodies[j].acceleration -= forceVec / m_bodies[j].mass;
        }
    }
}

// POPRAWIONE: właściwa implementacja Velocity Verlet
void Simulation::velocityVerletStep(double dt) {
    // Step 1: Update positions using current velocities and accelerations
    for (auto& body : m_bodies) {
        body.position += body.velocity * dt + body.acceleration * (0.5 * dt * dt);
    }
    
    // Store old accelerations for velocity update
    std::vector<Vec2> oldAccelerations;
    for (const auto& body : m_bodies) {
        oldAccelerations.push_back(body.acceleration);
    }
    
    // Step 2: Compute new accelerations
    computeGravityCPU();
    
    // Step 3: Update velocities using average of old and new accelerations
    for (size_t i = 0; i < m_bodies.size(); i++) {
        m_bodies[i].velocity += (oldAccelerations[i] + m_bodies[i].acceleration) * (0.5 * dt);
    }
}

void Simulation::computeGravityGPU() {
    // Fallback to CPU
    computeGravityCPU();
}