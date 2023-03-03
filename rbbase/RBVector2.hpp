//
//  RBVector2.hpp
//
//  Created by Roger Boesch on 30.07.18.
//  Copyright © 2018 Roger Boesch. All rights reserved.
//

#pragma once

#include <assert.h>
#include <cstdlib>

namespace rb {
    
    class Vector2 {
    public:
        union {
            float value[2];
            struct { float x, y; };
            struct { float r, g; };
            struct { float s, t; };
            struct { float width, height; };
            struct { float current, total; };
        };
        
        float& operator [](const size_t i) { assert(i < 2); return value[i]; }
        const float& operator [](const size_t i) const { assert(i < 2); return value[i]; }
        
        Vector2 operator +(const Vector2& b) const { return Vector2(x + b.x, y + b.y); }
        Vector2 operator -(const Vector2& b) const { return Vector2(x - b.x, y - b.y); }
        Vector2 operator *(const Vector2& b) const { return Vector2(x * b.x, y * b.y); }
        Vector2 operator /(const Vector2& b) const { return Vector2(x / b.x, y / b.y); }
        Vector2 operator -() const /* Unary minus */ { return Vector2(-x, -y); }
        Vector2 operator *(const float& b) const { return Vector2(x * b, y * b); }
        Vector2 operator /(const float& b) const { return Vector2(x / b, y / b); }
        
        bool operator ==(const Vector2& b) const { return (x == b.x) && (y == b.y); }
        bool operator !=(const Vector2& b) const { return !(*this == b); }
        
        Vector2& operator +=(const Vector2& b) { *this = *this + b; return *this; }
        Vector2& operator -=(const Vector2& b) { *this = *this - b; return *this; }
        Vector2& operator *=(const Vector2& b) { *this = *this * b; return *this; }
        Vector2& operator /=(const Vector2& b) { *this = *this / b; return *this; }
        Vector2& operator *=(const float& b) { *this = *this * b; return *this; }
        Vector2& operator /=(const float& b) { *this = *this / b; return *this; }
        
    public:
        explicit Vector2() : x(0), y(0) { }
        explicit Vector2(const float& n) : x(n), y(n) { }
        explicit Vector2(const float& nx, const float& ny) : x(nx), y(ny) { }
        
    public:
        float length() const;
        float lengthSquared() const;
        float distance(const Vector2 &p1) const;
        float distanceSquared(const Vector2 &p1) const;
        float dot(const Vector2 &v2) const;
        Vector2 normalize() const;
        
        bool isZero() { return x == 0 & y == 0; }
    };
    
    Vector2 operator* (const float &s, const Vector2 &v);
    
}


