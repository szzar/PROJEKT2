#pragma once

class ParabolicLayer{
    private:
        double peakHeightKm_;
        double halfThicknessKm_;
        double criticalFreqMHz_;

    public:
        ParabolicLayer();
        ParabolicLayer(double peakHeightKm, double halfThicknessKm, double criticalFreqMHz);

        double getPeakHeight() const;
        double getHalfThickness() const;
        double getCriticalFreq() const;
        bool reflects(double freqMHz) const;
        double virtualHeight(double freqMHz) const;
};
