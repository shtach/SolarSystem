#pragma once

#include "engine/core/Vec2.h"
#include <array>

class QuadTree {
public:
    QuadTree(Vec2 center, double halfSize);

    void reset(Vec2 center, double halfSize);

    void insert(int bodyIndex, const Vec2& position, double mass);

    void computeForce(int bodyIndex, const Vec2& position,
                      Vec2& outAcceleration,
                      double G, double theta, double softening) const;

private:
    struct BodyData {
        int    index{-1};
        Vec2   position{};
        double mass{0.0};
    };

    struct Node {
        Vec2   center;
        double halfSize;

        Vec2   centerOfMass{0.0, 0.0};
        double totalMass{0.0};

        static constexpr int MAX_BODIES = 4;
        std::array<BodyData, MAX_BODIES> bodies{};
        int bodyCount{0};

        std::array<Node*, 4> children{};

        Node() = default;
        Node(Vec2 c, double h) : center(c), halfSize(h) {}

        bool isLeaf() const { return children[0] == nullptr; }
    };

    // --- Node pool ---
    static constexpr int MAX_NODES = 8192;
    std::array<Node, MAX_NODES> m_pool{};
    int m_poolSize{0};

    Node* m_root{nullptr};

    Node* allocNode(Vec2 center, double halfSize);

    // --- Helpers ---
    static int  getQuadrant(const Node& node, const Vec2& position);
    void subdivide(Node& node);
    void insertIntoNode(Node& node,
                        int bodyIndex, const Vec2& position, double mass);

    static void computeForceNode(const Node& node,
                                 int bodyIndex, const Vec2& position,
                                 Vec2& acc,
                                 double G, double theta, double softening);
};