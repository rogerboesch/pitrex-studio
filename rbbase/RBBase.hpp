//
//  RBBase.hpp
//
//  Created by Roger Boesch on 19.08.18.
//  Copyright © 2018 Roger Boesch. All rights reserved.
//

#pragma once

#include "RBTimer.hpp"
#include <stdlib.h>

namespace rb {
    
    typedef uint32_t GLUnsignedInt;
    static unsigned long seed = 0;

    // -------------------------------------------------------------------------
    #pragma mark - Constants
    
    static const float kFramesPerSecond = 60.0;
    
    // -------------------------------------------------------------------------
    #pragma mark - Types & enums
    
    class primitives {
        
    public:
        typedef enum {
            Rectangle = 0,
            Circle,
            Triangle,
        } Shape2D;
        
        typedef enum {
            Box = 0,
            Pyramide,
            Container,      // Used in hierarchies
            Light           // Used for model lights
        } Shape3D;
    };
    
    class Rect {
    public:
        float x, y, w, h;
        
        explicit Rect() : x(0), y(0), w(0), h(0) { }
        explicit Rect(float x0, float y0, float w0, float h0) : x(x0), y(y0), w(w0), h(h0) { }
    };
    
    // -------------------------------------------------------------------------
    #pragma mark - Helper methods
    
    #ifndef M_PI
    #define M_PI 3.1415926535897932384626433832795f
    #endif
    
    inline unsigned int nextPot(unsigned int n) {
        n--;
        n |= n >> 1;
        n |= n >> 2;
        n |= n >> 4;
        n |= n >> 8;
        n |= n >> 16;
        
        n++;
        
        return n;
    }
    
    inline float degreesToRadian(float deg) {
        return M_PI * deg / 180.0;
    }
    
    inline float radiansToDegree(float rad) {
        return rad * 180.0 / M_PI;
    }
    
    inline float randomMax(int max) {
        if (seed == 0) {
            seed = Timer::current();
        }

        return rand() % (int)max;
    }

    inline int randomRange(int min, int max) {
        if (seed == 0) {
            seed = Timer::current();
        }

        int result = min + (rand() % static_cast<int>(max - min + 1));
        return result;
    }
    
    inline float randomFloat() {
        if (seed == 0) {
            seed = Timer::current();
        }
        
        float result = (((seed = 1664525*seed + 1013904223)>>16) / (float)0x10000);
        return result;
    }

    inline bool randomBool() {
        return randomRange(0, 1);
    }
    
    inline int distance(int x1, int y1, int x2, int y2) {
        const double x_diff = x2 - x1;
        const double y_diff = y2 - y1;
        return sqrt(x_diff * x_diff + y_diff * y_diff);
    }
}

