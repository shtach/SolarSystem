#pragma once
#include <cmath>

struct Vec2 {
    double x, y;

    Vec2(double x = 0.0, double y = 0.0) : x(x), y(y) {}

    Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(double scalar) const { return Vec2(x * scalar, y * scalar); }
    Vec2 operator/(double scalar) const { return Vec2(x / scalar, y / scalar); }

    Vec2& operator+=(const Vec2& other) { x += other.x; y += other.y; return *this; }
    Vec2& operator-=(const Vec2& other) { x -= other.x; y -= other.y; return *this; }
    Vec2& operator*=(double scalar) { x *= scalar; y *= scalar; return *this; }

    double length() const { return std::sqrt(x * x + y * y); }
    double lengthSquared() const { return x * x + y * y; }
    double dot(const Vec2& other) const { return x * other.x + y * other.y; }

    Vec2 normalized() const {
        double len = length();
        return len > 0.0 ? *this / len : Vec2();
    }
};