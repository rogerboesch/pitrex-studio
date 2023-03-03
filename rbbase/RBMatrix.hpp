//
//  RBMatrix.hpp
//
//  Created by Roger Boesch on 10.08.18.
//  Copyright © 2018 Roger Boesch. All rights reserved.
//

#pragma once

#include "RBVector3.hpp"

namespace rb {
    
    class Matrix44 {
    public:
        float m[4][4];
        
    public:
        Matrix44();
        
        void scale(float sx, float sy, float sz);
        void translate(float tx, float ty, float tz);
        void rotate(float angle, float x, float y, float z);
        
        void multiply(const Matrix44 &b);
        void copy(const Matrix44 &b);
        int  invert();
        void transpose(const Matrix44 &b);
        
        void loadIdentity();
        void frustum(float left, float right, float bottom, float top, float nearZ, float farZ);
        void perspective(float fovy, float aspect, float nearZ, float farZ);
        void ortho(float left, float right, float bottom, float top, float nearZ, float farZ);
        void lookAt(const Vector3 &eye, const Vector3 &target, const Vector3 &up);
    };
    
}


