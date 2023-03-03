//
//  RBLogger.hpp
//  Logging functionality
//
//  Created by Roger Boesch on 02/04/16.
//  Copyright © 2016 Roger Boesch All rights reserved.
//

#pragma once

#include <string>
#include <iostream>

namespace rb {
    
    // Print single entry
    template <typename T>
    void print(T t) {
        std::cout << t;
    }
    
    // Print when no arguments
    void doPrint();
    
    // Split the parameter pack
    // We want the first argument, so we can print it.
    // And the rest so we can forward it to the next call to doPrint()
    template <typename T, typename...Ts>
    void doPrint(T &&first, Ts&&... rest) {
        print(std::forward<T>(first));
        doPrint(std::forward<Ts>(rest)...);
    }
    
    class Logger {
    public:
        static const int DBG = 0;
        static const int INF = 1;
        static const int WRN = 2;
        static const int ERR = 3;
        static const int NONE = 4;
        
    public:
        static int getSeverity() { return _severity; }
        static void setSeverity(int severity) { _severity = severity; }
        
        template<typename T, typename... Args>
        static void error(T t, Args... args) {
            if (getSeverity() < _severity) {
                return;
            }
            
            doPrint("⛔️ ", t, args...);
            std::cout << std::endl;
        }
        
        template<typename T, typename... Args>
        static void warning(T t, Args... args) {
            if (getSeverity() < _severity) {
                return;
            }
            
            doPrint("⚠️ ", t, args...);
            std::cout << std::endl;
        }
        
        template<typename T, typename... Args>
        static void info(T t, Args... args) {
            if (getSeverity() < _severity) {
                return;
            }
            
            doPrint("▷ ", t, args...);
            std::cout << std::endl;
        }
        
        template<typename T, typename... Args>
        static void debug(T t, Args... args) {
            if (getSeverity() < _severity) {
                return;
            }
            
            doPrint("→ ", t, args...);
            std::cout << std::endl;
        }
        
        template<typename T, typename... Args>
        static void log(T t, Args... args) {
            doPrint("", t, args...);
            std::cout << std::endl;
        }
        
    private:
        static int _severity;
    };
    
}

