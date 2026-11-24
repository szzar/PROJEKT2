#include "Plot.h"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <utility>

namespace {

const sf::Color kInkColor(45, 45, 45);
const sf::Color kGridColor(218, 218, 212);

// Room around the data area for tick labels and axis titles
constexpr float kMarginLeft = 80.f; // (pixels)
constexpr float kMarginRight = 28.f;
constexpr float kMarginTop = 52.f;
constexpr float kMarginBottom = 64.f;

constexpr unsigned int kTitleSize = 16;
constexpr unsigned int kTickSize = 12;
constexpr unsigned int kLabelSize = 13;


constexpr const char* kFontPath = "gui/terminus.ttf";

std::string tickLabel(double value)
{
    return std::to_string(std::lround(value));
}

} // namespace

const sf::Font& defaultFont(){
    static sf::Font font;
    static const bool loaded = font.openFromFile(kFontPath);
    if (!loaded)
        throw std::runtime_error(std::string("Could not load font: ") + kFontPath);
    return font;
}

Plot::Plot(sf::FloatRect area, double xMin, double xMax, double yMin, double yMax,
           std::string title, std::string xLabel, std::string yLabel)
    : area_(area),
      xMin_(xMin), xMax_(xMax), yMin_(yMin), yMax_(yMax),
      title_(std::move(title)),
      xLabel_(std::move(xLabel)),
      yLabel_(std::move(yLabel))
{
}

void Plot::setTickCount(int xTicks, int yTicks)
{
    xTicks_ = std::max(1, xTicks);
    yTicks_ = std::max(1, yTicks);
}

void Plot::addSeries(std::vector<std::pair<double, double>> points, sf::Color color)
{
    series_.push_back(Series{std::move(points), color});
}

void Plot::draw(sf::RenderTarget& target) const
{
    if (xMax_ <= xMin_ || yMax_ <= yMin_)
        return;

    const sf::FloatRect plotArea(
        {area_.position.x + kMarginLeft, area_.position.y + kMarginTop},
        {area_.size.x - kMarginLeft - kMarginRight,
         area_.size.y - kMarginTop - kMarginBottom});

    if (plotArea.size.x <= 0.f || plotArea.size.y <= 0.f)
        return;

    const auto toPixel = [&](double x, double y) {
        const float fx = static_cast<float>((x - xMin_) / (xMax_ - xMin_));
        const float fy = static_cast<float>((y - yMin_) / (yMax_ - yMin_));
        return sf::Vector2f(plotArea.position.x + fx * plotArea.size.x,
                            plotArea.position.y + plotArea.size.y - fy * plotArea.size.y);
    };

    // Grid
    sf::VertexArray grid(sf::PrimitiveType::Lines);
    for (int i = 0; i <= xTicks_; ++i) {
        const double x = xMin_ + (xMax_ - xMin_) * (static_cast<double>(i) / xTicks_);
        grid.append(sf::Vertex{toPixel(x, yMin_), kGridColor});
        grid.append(sf::Vertex{toPixel(x, yMax_), kGridColor});
    }
    for (int i = 0; i <= yTicks_; ++i) {
        const double y = yMin_ + (yMax_ - yMin_) * (static_cast<double>(i) / yTicks_);
        grid.append(sf::Vertex{toPixel(xMin_, y), kGridColor});
        grid.append(sf::Vertex{toPixel(xMax_, y), kGridColor});
    }
    target.draw(grid);

    // Data lines
    for (const Series& series : series_) {
        sf::VertexArray line(sf::PrimitiveType::LineStrip);
        for (const std::pair<double, double>& point : series.points) {
            if (point.first < xMin_ || point.first > xMax_ ||
                point.second < yMin_ || point.second > yMax_)
                continue;
            line.append(sf::Vertex{toPixel(point.first, point.second), series.color});
        }
        target.draw(line);
    }

    // Border around the data area
    sf::RectangleShape border(plotArea.size);
    border.setPosition(plotArea.position);
    border.setFillColor(sf::Color::Transparent);
    border.setOutlineThickness(1.f);
    border.setOutlineColor(kInkColor);
    target.draw(border);

    const sf::Font& font = defaultFont();

    sf::Text title(font, title_, kTitleSize);
    title.setFillColor(kInkColor);
    {
        const sf::FloatRect bounds = title.getLocalBounds();
        title.setPosition(
            {area_.position.x + (area_.size.x - bounds.size.x) / 2.f - bounds.position.x,
             area_.position.y + 14.f - bounds.position.y});
    }
    target.draw(title);

    // Labelsland
    sf::Text tick(font, "", kTickSize);
    tick.setFillColor(kInkColor);

    for (int i = 0; i <= xTicks_; ++i) {
        const double x = xMin_ + (xMax_ - xMin_) * (static_cast<double>(i) / xTicks_);
        tick.setString(tickLabel(x));
        const sf::FloatRect bounds = tick.getLocalBounds();
        const sf::Vector2f p = toPixel(x, yMin_);
        tick.setPosition({p.x - bounds.size.x / 2.f - bounds.position.x, p.y + 8.f});
        target.draw(tick);
    }

    for (int i = 0; i <= yTicks_; ++i) {
        const double y = yMin_ + (yMax_ - yMin_) * (static_cast<double>(i) / yTicks_);
        tick.setString(tickLabel(y));
        const sf::FloatRect bounds = tick.getLocalBounds();
        const sf::Vector2f p = toPixel(xMin_, y);
        tick.setPosition({p.x - bounds.size.x - 10.f - bounds.position.x,
                          p.y - bounds.size.y / 2.f - bounds.position.y});
        target.draw(tick);
    }

    if (!xLabel_.empty()) {
        sf::Text label(font, xLabel_, kLabelSize);
        label.setFillColor(kInkColor);
        const sf::FloatRect bounds = label.getLocalBounds();
        label.setPosition(
            {plotArea.position.x + (plotArea.size.x - bounds.size.x) / 2.f - bounds.position.x,
             plotArea.position.y + plotArea.size.y + 34.f});
        target.draw(label);
    }

    if (!yLabel_.empty()) {
        sf::Text label(font, yLabel_, kLabelSize);
        label.setFillColor(kInkColor);
        label.setRotation(sf::degrees(-90.f));
        const sf::FloatRect bounds = label.getLocalBounds();
        label.setPosition({area_.position.x + 18.f,
                           plotArea.position.y + plotArea.size.y * 0.5f + bounds.size.x * 0.5f});
        target.draw(label);
    }
}
