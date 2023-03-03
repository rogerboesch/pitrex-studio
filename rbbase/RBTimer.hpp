//
//  RBTimer.hpp
//
//  Created by Roger Boesch on 11.03.18.
//  Copyright © 2018 Roger Boesch. All rights reserved.
//

#pragma once

#include <string>
#include <chrono>

namespace rb {
    
    class Timer {
    public:
        Timer();
        
        float delta();
        bool sync(float waitTime);

        static long current();
        
    private:
        bool _firstTime;
        std::chrono::high_resolution_clock::time_point _timestamp;
        std::chrono::high_resolution_clock::time_point _lastSync;
    };
    
}
