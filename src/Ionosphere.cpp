#include "Ionosphere.h"

#include <cmath>      // std::sqrt std::log
#include <limits>     // std::numeric_limits
#include <stdexcept>  // std::invalid_argument

ParabolicLayer::ParabolicLayer(): ParabolicLayer(300.0, 100.0, 7.0){}; // MADE UP VALS peakheight halfThickness criticalFreq
