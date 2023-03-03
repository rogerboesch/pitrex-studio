//
//  RBVector2.cpp
//
//  Created by Roger Boesch on 30.07.18.
//  Copyright © 2018 Roger Boesch. All rights reserved.
//

#include "RBVector2.hpp"
#include "math.h"

namespace rb {
    
    Vector2 operator* (const float &s, const Vector2 &v) {
        return v * s;
    }
    
    float Vector2::length() const {
        return sqrtf(lengthSquared());
    }
    
    float Vector2::lengthSquared() const {
        return dot(*this);
    }
    
    float Vector2::distance(const Vector2 &p1) const {
        return sqrtf((*this).distanceSquared(p1));
    }
    
    float Vector2::distanceSquared(const Vector2 &p1) const {
        return (p1 - (*this)).lengthSquared();
    }
    
    float Vector2::dot(const Vector2 &v2) const {
        return x * v2.x + y * v2.y;
    }
    
    Vector2 Vector2::normalize() const {
        return (*this) / length();
    }
    
}


