#pragma once

#include <string>
#include <utility>
#include <vector>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>

namespace sf { class Font; class RenderTarget; }

// Loads terminus
const sf::Font& defaultFont();

//PLot

class Plot
{
public:
    Plot(sf::FloatRect area, double xMin, double xMax, double yMin, double yMax,
         std::string title, std::string xLabel, std::string yLabel);

    void setTickCount(int xTicks, int yTicks);
    void addSeries(std::vector<std::pair<double, double>> points, sf::Color color);
    void draw(sf::RenderTarget& target) const;

private:
    struct Series
    {
        std::vector<std::pair<double, double>> points;
        sf::Color color;
    };

    sf::FloatRect area_;
    double xMin_;
    double xMax_;
    double yMin_;
    double yMax_;
    std::string title_;
    std::string xLabel_;
    std::string yLabel_;
    int xTicks_ = 5;
    int yTicks_ = 5;
    std::vector<Series> series_;
};
