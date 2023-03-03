//
//  RBVector3.hpp
//
//  Created by Roger Boesch on 30.07.18.
//  Copyright © 2018 Roger Boesch. All rights reserved.
//

#pragma once

#include <cstring>
#include <assert.h>

namespace rb {
    
    class Vector3 {
    public:
        union {
            float value[3];
            struct { float x, y, z; };
            struct { float w, h, l; };
        };
        
        float& operator [](const size_t i) { assert(i < 3); return value[i]; }
        const float& operator [](const size_t i) const { assert(i < 3); return value[i]; }
        
        bool operator ==(const Vector3& b) const { return (x == b.x) && (y == b.y) && (z == b.z); }
        bool operator !=(const Vector3& b) const { return !(*this == b); }
        Vector3 operator +(const Vector3& b) const { return Vector3(x + b.x, y + b.y, z + b.z); }
        Vector3 operator -(const Vector3& b) const { return Vector3(x - b.x, y - b.y, z - b.z); }

        Vector3 operator *=(const float f) { return Vector3(x*f, y*f, z*f); }
        Vector3 operator *=(const Vector3& v) { return Vector3(x*v.x, y*v.y, z*v.z); }

    public:
        explicit Vector3() : x(0), y(0), z(0) { }
        explicit Vector3(const float& n) : x(n), y(n), z(n) { }
        explicit Vector3(const float& nx, const float& ny, const float& nz) : x(nx), y(ny), z(nz) { }
        
    public:
        void normalize();
        void inverse();
        void scale(float scale);
        float length() const;
        float lengthSquared() const;
        float distance(const Vector3 &b) const;
        float distanceSquared(const Vector3 &b) const;
        
        Vector3 subtract(const Vector3 &b) const;
        Vector3 cross(const Vector3 &b) const;
        float dot(const Vector3 &b) const;
        
        Vector3 lerp(const Vector3 &b, float t) const;
        Vector3 directionTo(const Vector3 &destination) const;
        
    public:
        static Vector3 forward;
        static Vector3 up;
        static Vector3 right;
        
        static Vector3 one;
        static Vector3 zero;
    };
    
}


