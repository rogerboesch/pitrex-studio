//
//  RBColor.hpp
//
//  Created by Roger Boesch on 30.07.18.
//  Copyright © 2018 Roger Boesch. All rights reserved.
//

#pragma once

#include <cstring>
#include <assert.h>
#include <string>

namespace rb {
    
    class Color {
    public:
        union {
            float value[4];
            struct { float r, g, b, a; };
        };
        
        float& operator [](const size_t i) { assert(i < 2); return value[i]; }
        const float& operator [](const size_t i) const { assert(i < 2); return value[i]; }
 
    public:
        explicit Color() : r(0), g(0), b(0), a(1) { }
        explicit Color(const float& n) : r(n), g(n), b(n), a(1) { }
        explicit Color(const float& r, const float& g, const float& b, const float& a) : r(r), g(g), b(b), a(a) { }
        explicit Color(std::string hex);
        
    private:
        static int hexToDec(std::string hex);
        static Color hexToRGB(std::string hex);
        static void createPalette();
        
    public:
        Color alpha(float alpha);
        
    public:
        static Color random();
        static Color randomRange1(int min, int max);
        static Color randomRange2(int min, int max);
        static Color palette1(int index);
        static Color palette2(int index);

        static Color randomNeon();
        static Color randomGrayscale();
        static Color randomGreen();

    public:
        static Color Black;
        static Color White;
        static Color Gray;
        static Color GrayLight;
        static Color Red;
        static Color Green;
        static Color Blue;
        static Color Orange;
        static Color Cyan;
        static Color Yellow;
        static Color Purple;
    };
    
}

