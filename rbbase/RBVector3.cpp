//
//  RBVector3.cpp
//
//  Created by Roger Boesch on 10.08.18.
//  Copyright © 2018 Roger Boesch. All rights reserved.
//

#include "RBVector3.hpp"
#include <math.h>

namespace rb {
    
    Vector3 Vector3::forward = Vector3(0, 0, 1);
    Vector3 Vector3::up = Vector3 (0, 1, 0);
    Vector3 Vector3::right = Vector3(1, 0, 0);
    Vector3 Vector3::one = Vector3(1, 1, 1);
    Vector3 Vector3::zero = Vector3(0, 0, 0);

    void Vector3::normalize() {
        float len = length();
        
        if (len != 0) {
            len = 1.0 / len;
            
            x *= len;
            y *= len;
            z *= len;
        }
    }
    
    void Vector3::scale(float scale) {
        x *= scale;
        y *= scale;
        z *= scale;
    }
    
    void Vector3::inverse() {
        x = -x;
        y = -y;
        z = -z;
    }
    
    Vector3 Vector3::directionTo(const Vector3 &destination) const {
        Vector3 result;
        result.x = destination.x - x;
        result.y = destination.y - y;
        result.z = destination.z - z;
        
        result.normalize();
        return result;
    }
    
    float Vector3::length() const {
        return (float)sqrt(x * x + y * y + z * z);
    }
    
    float Vector3::lengthSquared() const {
        return (x * x + y * y + z * z);
    }
    
    float Vector3::distance(const Vector3 &b) const {
        Vector3 result = *this -b;
        return result.length();
    }
    
    float Vector3::distanceSquared(const Vector3 &b) const {
        Vector3 result = *this -b;
        return (result.x * result.x + result.y * result.y + result.z * result.z);
    }
    
    Vector3 Vector3::subtract(const Vector3 &b) const {
        Vector3 result;
        
        result.x = x - b.x;
        result.y = y - b.y;
        result.z = z - b.z;
        
        return result;
    }
    
    Vector3 Vector3::cross(const Vector3 &b) const {
        Vector3 result;
        
        result.x = y * b.z - z * b.y;
        result.y = z * b.x - x * b.z;
        result.z = x * b.y - b.y * x;
        
        return result;
    }
    
    float Vector3::dot(const Vector3 &b) const {
        return (x * b.x + y * b.y + z * b.z);
    }
    
    Vector3 Vector3::lerp(const Vector3 &b, float t) const {
        Vector3 result;
        
        result.x = (x * (1 - t) + b.x * t);
        result.y = (y * (1 - t) + b.y * t);
        result.z = (z * (1 - t) + b.z * t);
        
        return result;
    }
    
}

