//
//  RBTimer.cpp
//
//  Created by Roger Boesch on 03.09.18.
//  Copyright © 2018 Roger Boesch. All rights reserved.
//

#include "RBTimer.hpp"
#include <chrono>

namespace rb {
    
    Timer::Timer() {
        _firstTime = true;
    }

    float Timer::delta() {
        if (_firstTime) {
            _timestamp = std::chrono::high_resolution_clock::now();
            _lastSync = std::chrono::high_resolution_clock::now();
            _firstTime = false;

            return 0.0f;
        }

        std::chrono::duration<float> fs = std::chrono::high_resolution_clock::now() - _timestamp;
        _timestamp = std::chrono::high_resolution_clock::now();

        return fs.count();
    }

    bool Timer::sync(float waitTime) {
        if (_firstTime) {
            _timestamp = std::chrono::high_resolution_clock::now();
            _lastSync = std::chrono::high_resolution_clock::now();
            _firstTime = false;
        }
        
        std::chrono::duration<float> fs = std::chrono::high_resolution_clock::now() - _lastSync;
        float elapsed = fs.count();

        if (elapsed >= waitTime) {
            _lastSync = std::chrono::high_resolution_clock::now();
            return true;
        }
        
        return false;
    }

    long Timer::current() {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        return (long)now.time_since_epoch().count();
    }

}
