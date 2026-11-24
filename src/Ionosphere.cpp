#include "Ionosphere.h"

#include <cmath>
#include <limits>
#include <stdexcept>

ParabolicLayer::ParabolicLayer()
    : ParabolicLayer(300.0, 100.0, 7.0)
{
}

ParabolicLayer::ParabolicLayer(double peakHeightKm, double halfThicknessKm, double criticalFreqMHz)
    : peakHeightKm_(peakHeightKm),
      halfThicknessKm_(halfThicknessKm),
      criticalFreqMHz_(criticalFreqMHz)
{
    if (halfThicknessKm_ <= 0.0)
        throw std::invalid_argument("ParabolicLayer: half thickness must be positive");
    if (criticalFreqMHz_ <= 0.0)
        throw std::invalid_argument("ParabolicLayer: critical frequency must be positive");
}

double ParabolicLayer::getPeakHeight() const { return peakHeightKm_; }
double ParabolicLayer::getHalfThickness() const { return halfThicknessKm_; }
double ParabolicLayer::getCriticalFrequency() const { return criticalFreqMHz_; }

bool ParabolicLayer::reflects(double freqMHz) const
{
    return freqMHz > 0.0 && freqMHz < criticalFreqMHz_;
}

double ParabolicLayer::virtualHeight(double freqMHz) const
{
    if (!reflects(freqMHz))
        return std::numeric_limits<double>::quiet_NaN();

    //   X = (fc/f)^2 * (1 - u^2),  u = (h - hm)/ym
    //   h' = INT ym * a / sqrt(u^2 - (1 - a^2)) du,  a = f/fc
    //      = (hm - ym) + (ym/2) * a * ln((1 + a)/(1 - a))
    const double a = freqMHz / criticalFreqMHz_;
    return (peakHeightKm_ - halfThicknessKm_)
         + 0.5 * halfThicknessKm_ * a * std::log((1.0 + a) / (1.0 - a));
}
