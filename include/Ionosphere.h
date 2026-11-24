#pragma once

// Single parabolic-layer model of the ionosphere
//
// Plasma frequency profile:
// fN(h) = fc * sqrt(1 - ((h - hm) / ym)^2),  |h - hm| <= ym
// with hm = peak height (km), ym = half thickness (km), fc = critical frequency (MHz).

// For f < fc the virtual height integral has a closed form:
// h'(f) = (hm - ym) + (ym/2) * (f/fc) * ln((1 + f/fc) / (1 - f/fc))
// and h' diverges at f = fc (penetration), which is the vertical tail at the right edge of a real ionogram.

class ParabolicLayer
{
public:
    ParabolicLayer();
    ParabolicLayer(double peakHeightKm, double halfThicknessKm, double criticalFreqMHz);

    double getPeakHeight() const;
    double getHalfThickness() const;
    double getCriticalFrequency() const;

    bool reflects(double freqMHz) const;         // true while freq < fc
    double virtualHeight(double freqMHz) const;  // km; NaN if freq >= fc

private:
    double peakHeightKm_;
    double halfThicknessKm_;
    double criticalFreqMHz_;
};
