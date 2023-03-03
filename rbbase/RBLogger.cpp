//
//  RBLogger.cpp
//  Logging functionality
//
//  Created by Roger Boesch on 02/04/16.
//  Copyright © 2016 Roger Boesch All rights reserved.
//

#include "RBLogger.hpp"

#include <iostream>

namespace rb {
    
    int Logger::_severity = Logger::DBG;
    
    // Print when no arguments
    void doPrint() {}
    
}

