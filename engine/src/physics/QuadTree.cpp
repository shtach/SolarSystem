#include "engine/physics/QuadTree.h"
#include <cmath>

// --- Pool allocation ---
QuadTree::Node* QuadTree::allocNode(Vec2 center, double halfSize) {
    Node& n = m_pool[m_poolSize++];
    n = Node(center, halfSize); // reset
    return &n;
}

// --- Public API ---

QuadTree::QuadTree(Vec2 center, double halfSize) {
    reset(center, halfSize);
}

void QuadTree::reset(Vec2 center, double halfSize) {
    m_poolSize = 0;
    m_root = allocNode(center, halfSize);
}

void QuadTree::insert(int bodyIndex, const Vec2& position, double mass) {
    insertIntoNode(*m_root, bodyIndex, position, mass);
}

void QuadTree::computeForce(int bodyIndex, const Vec2& position,
                            Vec2& outAcceleration,
                            double G, double theta, double softening) const {
    computeForceNode(*m_root, bodyIndex, position,
                     outAcceleration, G, theta, softening);
}

// --- Helpers ---

int QuadTree::getQuadrant(const Node& node, const Vec2& position) {
    bool right = position.x >= node.center.x;
    bool up    = position.y >= node.center.y;

    if ( right &&  up) return 0; // NE
    if (!right &&  up) return 1; // NW
    if (!right && !up) return 2; // SW
    return 3;                    // SE
}

void QuadTree::subdivide(Node& node) {
    const double h = node.halfSize * 0.5;
    const Vec2&  c = node.center;

    node.children[0] = allocNode(Vec2(c.x + h, c.y + h), h);
    node.children[1] = allocNode(Vec2(c.x - h, c.y + h), h);
    node.children[2] = allocNode(Vec2(c.x - h, c.y - h), h);
    node.children[3] = allocNode(Vec2(c.x + h, c.y - h), h);

    // Redistribute bodies
    for (int i = 0; i < node.bodyCount; ++i) {
        const BodyData& b = node.bodies[i];
        int q = getQuadrant(node, b.position);
        insertIntoNode(*node.children[q], b.index, b.position, b.mass);
    }

    node.bodyCount = 0; // now internal
}

void QuadTree::insertIntoNode(Node& node,
                               int bodyIndex,
                               const Vec2& position,
                               double mass) {
    // Update center of mass
    const double newMass = node.totalMass + mass;
    node.centerOfMass =
        (node.centerOfMass * node.totalMass + position * mass) * (1.0 / newMass);
    node.totalMass = newMass;

    if (node.isLeaf()) {
        if (node.bodyCount < Node::MAX_BODIES) {
            node.bodies[node.bodyCount++] = {bodyIndex, position, mass};
            return;
        }
        // full → subdivide
        subdivide(node);
    }

    // recurse
    int q = getQuadrant(node, position);
    insertIntoNode(*node.children[q], bodyIndex, position, mass);
}

void QuadTree::computeForceNode(const Node& node,
                                 int bodyIndex,
                                 const Vec2& position,
                                 Vec2& acc,
                                 double G,
                                 double theta,
                                 double softening) {
    if (node.totalMass == 0.0) return;

    const Vec2 delta  = node.centerOfMass - position;
    const double distSq = delta.lengthSquared() + softening;
    const double dist   = std::sqrt(distSq);

    if (node.isLeaf()) {
        for (int i = 0; i < node.bodyCount; ++i) {
            const BodyData& b = node.bodies[i];
            if (b.index == bodyIndex) continue;

            const Vec2 d = b.position - position;
            const double dSq = d.lengthSquared() + softening;
            const double inv = 1.0 / (dSq * std::sqrt(dSq));

            acc += d * (G * b.mass * inv);
        }
        return;
    }

    // Barnes-Hut condition
    const double s = node.halfSize * 2.0;
    if ((s / dist) < theta) {
        const double inv = 1.0 / (distSq * dist);
        acc += delta * (G * node.totalMass * inv);
        return;
    }

    // recurse
    for (const Node* child : node.children) {
        if (child) {
            computeForceNode(*child, bodyIndex, position,
                             acc, G, theta, softening);
        }
    }
}