#include "Ionosphere.h"

#include <cmath>      // std::sqrt std::log
#include <limits>     // std::numeric_limits
#include <stdexcept>  // std::invalid_argument

ParabolicLayer::ParabolicLayer(): ParabolicLayer(300.0, 100.0, 7.0){}; // MADE UP VALS peakheight halfThickness criticalFreq

ParabolicLayer::ParabolicLayer(double peakHeightKm, double halfThicknessKm, double criticalFreqMHz): peakHeightKm_(peakHeightKm),halfThicknessKm_(halfThicknessKm),criticalFreqMHz_(criticalFreqMHz){};


double ParabolicLayer::getPeakHeight() const { return peakHeightKm_; }
double ParabolicLayer::getHalfThickness() const { return halfThicknessKm_; }
double ParabolicLayer::getCriticalFreq() const { return criticalFreqMHz_; }

bool ParabolicLayer::reflects(double freqMHz) const{
    return freqMHz > 0.0 && freqMHz < criticalFreqMHz_;
}

double ParabolicLayer::virtualHeight(double freqMHz) const{
    if (!reflects(freqMHz))
        return std::numeric_limits<double>::quiet_NaN();
    const double a = freqMHz / criticalFreqMHz_;
    return (peakHeightKm_ - halfThicknessKm_)
         + 0.5 * halfThicknessKm_ * a * std::log((1.0 + a) / (1.0 - a));
}
