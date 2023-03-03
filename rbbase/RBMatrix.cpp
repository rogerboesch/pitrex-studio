//
//  RBMatrix.cpp
//
//  Created by Roger Boesch on 10.08.18.
//  Copyright © 2018 Roger Boesch. All rights reserved.
//

#include "RBMatrix.hpp"

#include <stdlib.h>
#include <math.h>

namespace rb {
    
    // -------------------------------------------------------------------------
    #pragma mark - Initialisation
    
    Matrix44::Matrix44() {
        memset(m, 0x0, sizeof(Matrix44));
    }
    
    void Matrix44::loadIdentity() {
        memset(m, 0x0, sizeof(Matrix44));
        
        m[0][0] = 1.0f;
        m[1][1] = 1.0f;
        m[2][2] = 1.0f;
        m[3][3] = 1.0f;
    }
    
    // -------------------------------------------------------------------------
    #pragma mark - Transform
    
    // Multiply matrix specified by result with a scaling matrix and return new matrix in result
    // result Specifies the input matrix. Scaled matrix is returned in result.
    // sx, sy, sz Scale factors along the x, y and z axes respectively
    void Matrix44::scale(float sx, float sy, float sz) {
        m[0][0] *= sx;
        m[0][1] *= sx;
        m[0][2] *= sx;
        m[0][3] *= sx;
        
        m[1][0] *= sy;
        m[1][1] *= sy;
        m[1][2] *= sy;
        m[1][3] *= sy;
        
        m[2][0] *= sz;
        m[2][1] *= sz;
        m[2][2] *= sz;
        m[2][3] *= sz;
    }
    
    // Multiply matrix specified by result with a translation matrix and return new matrix in result
    // result Specifies the input matrix. Translated matrix is returned in result.
    // tx, ty, tz Scale factors along the x, y and z axes respectively
    void Matrix44::translate(float tx, float ty, float tz) {
        m[3][0] += (m[0][0] * tx + m[1][0] * ty + m[2][0] * tz);
        m[3][1] += (m[0][1] * tx + m[1][1] * ty + m[2][1] * tz);
        m[3][2] += (m[0][2] * tx + m[1][2] * ty + m[2][2] * tz);
        m[3][3] += (m[0][3] * tx + m[1][3] * ty + m[2][3] * tz);
    }
    
    // Multiply matrix specified by result with a rotation matrix and return new matrix in result
    // result Specifies the input matrix.  Rotated matrix is returned in result.
    // angle Specifies the angle of rotation, in degrees.
    // x, y, z Specify the x, y and z coordinates of a vector, respectively
    void Matrix44::rotate(float angle, float x, float y, float z) {
        float sinAngle, cosAngle;
        float mag = sqrtf(x * x + y * y + z * z);
        
        sinAngle = sinf ( angle * M_PI / 180.0f );
        cosAngle = cosf ( angle * M_PI / 180.0f );
        
        if (mag > 0.0f) {
            float xx, yy, zz, xy, yz, zx, xs, ys, zs;
            float oneMinusCos;
            Matrix44 rotMat;
            
            x /= mag;
            y /= mag;
            z /= mag;
            
            xx = x * x;
            yy = y * y;
            zz = z * z;
            xy = x * y;
            yz = y * z;
            zx = z * x;
            xs = x * sinAngle;
            ys = y * sinAngle;
            zs = z * sinAngle;
            oneMinusCos = 1.0f - cosAngle;
            
            rotMat.m[0][0] = (oneMinusCos * xx) + cosAngle;
            rotMat.m[0][1] = (oneMinusCos * xy) - zs;
            rotMat.m[0][2] = (oneMinusCos * zx) + ys;
            rotMat.m[0][3] = 0.0F;
            
            rotMat.m[1][0] = (oneMinusCos * xy) + zs;
            rotMat.m[1][1] = (oneMinusCos * yy) + cosAngle;
            rotMat.m[1][2] = (oneMinusCos * yz) - xs;
            rotMat.m[1][3] = 0.0F;
            
            rotMat.m[2][0] = (oneMinusCos * zx) - ys;
            rotMat.m[2][1] = (oneMinusCos * yz) + xs;
            rotMat.m[2][2] = (oneMinusCos * zz) + cosAngle;
            rotMat.m[2][3] = 0.0F;
            
            rotMat.m[3][0] = 0.0F;
            rotMat.m[3][1] = 0.0F;
            rotMat.m[3][2] = 0.0F;
            rotMat.m[3][3] = 1.0F;
            
            rotMat.multiply(*this);
            memcpy(m, &rotMat, sizeof(Matrix44));
        }
    }
    
    // -------------------------------------------------------------------------
    #pragma mark - Manipulation
    
    // Perform the following operation - result matrix = srcA matrix * srcB matrix
    // result Returns multiplied matrix
    // srcA, srcB Input matrices to be multiplied
    void Matrix44::multiply(const Matrix44 &b) {
        Matrix44 tmp;
        int i;
        
        for (i = 0; i < 4; i++) {
            tmp.m[i][0] = (m[i][0] * b.m[0][0]) +
            (m[i][1] * b.m[1][0]) +
            (m[i][2] * b.m[2][0]) +
            (m[i][3] * b.m[3][0]) ;
            
            tmp.m[i][1] = (m[i][0] * b.m[0][1]) +
            (m[i][1] * b.m[1][1]) +
            (m[i][2] * b.m[2][1]) +
            (m[i][3] * b.m[3][1]) ;
            
            tmp.m[i][2] = (m[i][0] * b.m[0][2]) +
            (m[i][1] * b.m[1][2]) +
            (m[i][2] * b.m[2][2]) +
            (m[i][3] * b.m[3][2]) ;
            
            tmp.m[i][3] = (m[i][0] * b.m[0][3]) +
            (m[i][1] * b.m[1][3]) +
            (m[i][2] * b.m[2][3]) +
            (m[i][3] * b.m[3][3]) ;
        }
        
        memcpy(m, &tmp, sizeof(Matrix44));
    }
    
    int Matrix44::invert() {
        Matrix44 in;
        in.copy(*this);
        
        Matrix44 out;
        float *m = (float *)(&in.m[0][0]);
        float *om = (float *)(&out.m[0][0]);
        double inv[16], det;
        int i;
        
        inv[0] = m[5]  * m[10] * m[15] -
        m[5]  * m[11] * m[14] -
        m[9]  * m[6]  * m[15] +
        m[9]  * m[7]  * m[14] +
        m[13] * m[6]  * m[11] -
        m[13] * m[7]  * m[10];
        
        inv[4] = -m[4]  * m[10] * m[15] +
        m[4]  * m[11] * m[14] +
        m[8]  * m[6]  * m[15] -
        m[8]  * m[7]  * m[14] -
        m[12] * m[6]  * m[11] +
        m[12] * m[7]  * m[10];
        
        inv[8] = m[4]  * m[9] * m[15] -
        m[4]  * m[11] * m[13] -
        m[8]  * m[5] * m[15] +
        m[8]  * m[7] * m[13] +
        m[12] * m[5] * m[11] -
        m[12] * m[7] * m[9];
        
        inv[12] = -m[4]  * m[9] * m[14] +
        m[4]  * m[10] * m[13] +
        m[8]  * m[5] * m[14] -
        m[8]  * m[6] * m[13] -
        m[12] * m[5] * m[10] +
        m[12] * m[6] * m[9];
        
        inv[1] = -m[1]  * m[10] * m[15] +
        m[1]  * m[11] * m[14] +
        m[9]  * m[2] * m[15] -
        m[9]  * m[3] * m[14] -
        m[13] * m[2] * m[11] +
        m[13] * m[3] * m[10];
        
        inv[5] = m[0]  * m[10] * m[15] -
        m[0]  * m[11] * m[14] -
        m[8]  * m[2] * m[15] +
        m[8]  * m[3] * m[14] +
        m[12] * m[2] * m[11] -
        m[12] * m[3] * m[10];
        
        inv[9] = -m[0]  * m[9] * m[15] +
        m[0]  * m[11] * m[13] +
        m[8]  * m[1] * m[15] -
        m[8]  * m[3] * m[13] -
        m[12] * m[1] * m[11] +
        m[12] * m[3] * m[9];
        
        inv[13] = m[0]  * m[9] * m[14] -
        m[0]  * m[10] * m[13] -
        m[8]  * m[1] * m[14] +
        m[8]  * m[2] * m[13] +
        m[12] * m[1] * m[10] -
        m[12] * m[2] * m[9];
        
        inv[2] = m[1]  * m[6] * m[15] -
        m[1]  * m[7] * m[14] -
        m[5]  * m[2] * m[15] +
        m[5]  * m[3] * m[14] +
        m[13] * m[2] * m[7] -
        m[13] * m[3] * m[6];
        
        inv[6] = -m[0]  * m[6] * m[15] +
        m[0]  * m[7] * m[14] +
        m[4]  * m[2] * m[15] -
        m[4]  * m[3] * m[14] -
        m[12] * m[2] * m[7] +
        m[12] * m[3] * m[6];
        
        inv[10] = m[0]  * m[5] * m[15] -
        m[0]  * m[7] * m[13] -
        m[4]  * m[1] * m[15] +
        m[4]  * m[3] * m[13] +
        m[12] * m[1] * m[7] -
        m[12] * m[3] * m[5];
        
        inv[14] = -m[0]  * m[5] * m[14] +
        m[0]  * m[6] * m[13] +
        m[4]  * m[1] * m[14] -
        m[4]  * m[2] * m[13] -
        m[12] * m[1] * m[6] +
        m[12] * m[2] * m[5];
        
        inv[3] = -m[1] * m[6] * m[11] +
        m[1] * m[7] * m[10] +
        m[5] * m[2] * m[11] -
        m[5] * m[3] * m[10] -
        m[9] * m[2] * m[7] +
        m[9] * m[3] * m[6];
        
        inv[7] = m[0] * m[6] * m[11] -
        m[0] * m[7] * m[10] -
        m[4] * m[2] * m[11] +
        m[4] * m[3] * m[10] +
        m[8] * m[2] * m[7] -
        m[8] * m[3] * m[6];
        
        inv[11] = -m[0] * m[5] * m[11] +
        m[0] * m[7] * m[9] +
        m[4] * m[1] * m[11] -
        m[4] * m[3] * m[9] -
        m[8] * m[1] * m[7] +
        m[8] * m[3] * m[5];
        
        inv[15] = m[0] * m[5] * m[10] -
        m[0] * m[6] * m[9] -
        m[4] * m[1] * m[10] +
        m[4] * m[2] * m[9] +
        m[8] * m[1] * m[6] -
        m[8] * m[2] * m[5];
        
        det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];
        
        if (det == 0) {
            return 0;
        }
        
        det = 1.0 / det;
        for (i = 0; i < 16; i++) {
            *om++ = (float)(inv[i] * det);
        }
        
        memcpy(m, &out, sizeof(Matrix44));
        
        return 1;
    }
    
    void Matrix44::transpose(const Matrix44 &b) {
        Matrix44 tmp;
        tmp.m[0][0] = b.m[0][0];
        tmp.m[0][1] = b.m[1][0];
        tmp.m[0][2] = b.m[2][0];
        tmp.m[0][3] = b.m[3][0];
        
        tmp.m[1][0] = b.m[0][1];
        tmp.m[1][1] = b.m[1][1];
        tmp.m[1][2] = b.m[2][1];
        tmp.m[1][3] = b.m[3][1];
        
        tmp.m[2][0] = b.m[0][2];
        tmp.m[2][1] = b.m[1][2];
        tmp.m[2][2] = b.m[2][2];
        tmp.m[2][3] = b.m[3][2];
        
        tmp.m[3][0] = b.m[0][3];
        tmp.m[3][1] = b.m[1][3];
        tmp.m[3][2] = b.m[2][3];
        tmp.m[3][3] = b.m[3][3];
        
        memcpy(m, &tmp, sizeof(Matrix44));
    }
    
    // -------------------------------------------------------------------------
    #pragma mark - High level
    
    void Matrix44::frustum(float left, float right, float bottom, float top, float nearZ, float farZ) {
        float deltaX = right - left;
        float deltaY = top - bottom;
        float deltaZ = farZ - nearZ;
        Matrix44 frust;
        
        if ((nearZ <= 0.0f) || (farZ <= 0.0f) ||
            (deltaX <= 0.0f) || (deltaY <= 0.0f) || (deltaZ <= 0.0f)) {
            return;
        }
        
        frust.m[0][0] = 2.0f * nearZ / deltaX;
        frust.m[0][1] = frust.m[0][2] = frust.m[0][3] = 0.0f;
        
        frust.m[1][1] = 2.0f * nearZ / deltaY;
        frust.m[1][0] = frust.m[1][2] = frust.m[1][3] = 0.0f;
        
        frust.m[2][0] = (right + left) / deltaX;
        frust.m[2][1] = (top + bottom) / deltaY;
        frust.m[2][2] = -(nearZ + farZ) / deltaZ;
        frust.m[2][3] = -1.0f;
        
        frust.m[3][2] = -2.0f * nearZ * farZ / deltaZ;
        frust.m[3][0] = frust.m[3][1] = frust.m[3][3] = 0.0f;
        
        frust.multiply(*this);
        memcpy(m, &frust, sizeof(Matrix44));
    }
    
    // Multiply matrix specified by result with a perspective matrix and return new matrix in result
    // result Specifies the input matrix. new matrix is returned in result.
    // fovy Field of view y angle in degrees
    // aspect Aspect ratio of screen
    // nearZ Near plane distance
    // farZ Far plane distance
    void Matrix44::perspective(float fovy, float aspect, float nearZ, float farZ) {
        float frustumW, frustumH;
        
        frustumH = tanf(fovy / 360.0f * M_PI) * nearZ;
        frustumW = frustumH * aspect;
        
        frustum(-frustumW, frustumW, -frustumH, frustumH, nearZ, farZ);
    }
    
    // Multiply matrix specified by result with a perspective matrix and return new matrix in result
    // result Specifies the input matrix.  new matrix is returned in result.
    // left, right Coordinates for the left and right vertical clipping planes
    // bottom, top Coordinates for the bottom and top horizontal clipping planes
    // nearZ, farZ Distances to the near and far depth clipping planes.  These values are negative if plane is behind the viewer
    void Matrix44::ortho(float left, float right, float bottom, float top, float nearZ, float farZ) {
        float deltaX = right - left;
        float deltaY = top - bottom;
        float deltaZ = farZ - nearZ;
        
        if ((deltaX == 0.0f) || (deltaY == 0.0f) || (deltaZ == 0.0f)) {
            return;
        }
        
        m[0][0] = 2.0/deltaX;
        m[0][3] = -1.0;
        m[1][1] = 2.0/deltaY;
        m[1][3] = -1.0;
        m[2][2] = -2.0/(farZ-nearZ);
        m[3][3] = 1.0;
    }
    
    void Matrix44::lookAt(const Vector3 &eye, const Vector3 &target, const Vector3 &up) {
        Vector3 side, up2, forward;
        Matrix44 transMat;
        
        forward = target - eye;
        forward.normalize();
        
        side = forward.cross(up);
        side.normalize();
        
        up2 = forward.cross(side);
        up2.normalize();
        
        Matrix44 result;
        result.loadIdentity();
        result.m[0][0] = side.x;
        result.m[0][1] = side.y;
        result.m[0][2] = side.z;
        result.m[1][0] = up2.x;
        result.m[1][1] = up2.y;
        result.m[1][2] = up2.z;
        result.m[2][0] = -forward.x;
        result.m[2][1] = -forward.y;
        result.m[2][2] = -forward.z;
        
        transMat.loadIdentity();
        transMat.translate(-eye.x, -eye.y, -eye.z);
        transMat.multiply(result);
        
        //    result.Translate(-eye.x, -eye.y, -eye.z);
        
        memcpy(m, &transMat, sizeof(Matrix44));
    }
    
    // -------------------------------------------------------------------------
    #pragma mark - Helper
    
    void Matrix44::copy(const Matrix44 &src) {
        memcpy(m, &src, sizeof(Matrix44));
    }
    
}

