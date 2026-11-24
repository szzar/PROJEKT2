#include "Instrument.h"
#include "Plot.h"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <memory>
#include <utility>

// Instrument

Instrument::Instrument() = default;

Instrument::Instrument(std::string name, int id): name_(std::move(name)), id_(id){}

const std::string& Instrument::getName() const { return name_; }
int Instrument::getId() const { return id_; }
void Instrument::setName(const std::string& name) { name_ = name; }
void Instrument::setId(int id) { id_ = id; }

// Ionosonde

Ionosonde::Ionosonde() : Instrument("Ionosonde", 0){}

Ionosonde::Ionosonde(std::string name, int id, const ParabolicLayer& layer,double startFreqMHz, double endFreqMHz, int sampleCount)
    :Instrument(std::move(name), id),
      layer_(layer),
      startFreqMHz_(startFreqMHz),
      endFreqMHz_(endFreqMHz),
      sampleCount_(sampleCount > 1 ? sampleCount : 2){}

std::string Ionosonde::getTypeName() const { return "Ionosonde"; }

std::vector<std::pair<double, double>> Ionosonde::computeIonogram() const{
    std::vector<std::pair<double, double>> points;
    const double step = (endFreqMHz_ - startFreqMHz_) / static_cast<double>(sampleCount_ - 1);

    for (int i = 0; i < sampleCount_; ++i) {
        const double frequency = startFreqMHz_ + step * i;
        if (!layer_.reflects(frequency))
            break;  // above the critical frequency the wave penetrates
        points.emplace_back(frequency, layer_.virtualHeight(frequency));
    }
    return points;
}

void Ionosonde::draw(sf::RenderTarget& target, const sf::FloatRect& area) const{
    const double xMax =
        std::max(1.0, std::ceil(std::max(endFreqMHz_, layer_.getCriticalFrequency())));

    Plot plot(area, 0.0, xMax, 0.0, 600.0,
              "Ionogram - " + getName(),
              "Frequency (MHz)", "Virtual height (km)");
    plot.setTickCount(static_cast<int>(xMax), 6);
    plot.addSeries(computeIonogram(), sf::Color(178, 40, 40));
    plot.draw(target);
}

// WeatherStation

WeatherStation::WeatherStation():location_("Unknown"){}

WeatherStation::WeatherStation(std::string name, int id, std::string location):Instrument(std::move(name), id), location_(std::move(location)){}

std::string WeatherStation::getTypeName() const { return "Weather station"; }

void WeatherStation::addReading(double hour, double temperatureC){
    readings_.push_back(WeatherReading{hour, temperatureC});
    std::sort(readings_.begin(), readings_.end(),
              [](const WeatherReading& a, const WeatherReading& b) { return a.hour < b.hour; });
}

bool WeatherStation::removeReading(std::size_t index){
    if (index >= readings_.size())
        return false;
    readings_.erase(readings_.begin() + static_cast<std::ptrdiff_t>(index));
    return true;
}

bool WeatherStation::setReadingTemperature(std::size_t index, double temperatureC){
    if (index >= readings_.size())
        return false;
    readings_[index].temperatureC = temperatureC;
    return true;
}

int WeatherStation::findReading(double hour) const{
    for (std::size_t i = 0; i < readings_.size(); ++i) {
        if (std::abs(readings_[i].hour - hour) < 1e-9)
            return static_cast<int>(i);
    }
    return -1;
}

const WeatherReading& WeatherStation::getReading(std::size_t index) const{
    return readings_.at(index);
}

double WeatherStation::getMinTemperature() const{
    if (readings_.empty())
        return 0.0;

    double minimum = readings_.front().temperatureC;
    for (const WeatherReading& reading : readings_)
        minimum = std::min(minimum, reading.temperatureC);
    return minimum;
}

double WeatherStation::getMaxTemperature() const{
    if (readings_.empty())
        return 0.0;

    double maximum = readings_.front().temperatureC;
    for (const WeatherReading& reading : readings_)
        maximum = std::max(maximum, reading.temperatureC);
    return maximum;
}

const std::string& WeatherStation::getLocation() const { return location_; }
void WeatherStation::setLocation(const std::string& location) { location_ = location; }

void WeatherStation::draw(sf::RenderTarget& target, const sf::FloatRect& area) const{
    std::vector<std::pair<double, double>> points;
    points.reserve(readings_.size());
    for (const WeatherReading& reading : readings_)
        points.emplace_back(reading.hour, reading.temperatureC);

    double yMin = 0.0;
    double yMax = 30.0;
    if (!readings_.empty()) {
        yMin = 5.0 * std::floor(getMinTemperature() / 5.0);
        yMax = 5.0 * std::ceil(getMaxTemperature() / 5.0);
        if (yMax <= yMin)
            yMax = yMin + 5.0;
    }

    Plot plot(area, 0.0, 24.0, yMin, yMax,
              "24h temperature - " + getName() + " (" + getLocation() + ")",
              "Hour", "Temperature (C)");
    plot.setTickCount(8, 3);
    plot.addSeries(std::move(points), sf::Color(30, 90, 190));
    plot.draw(target);
}

// CameraFeed

namespace {
bool loadTexture(sf::Texture& texture, const std::string& path){
    const std::string candidates[] = {path, "../" + path};

    for (const std::string& candidate : candidates) {
        std::error_code ignored;
        if (!std::filesystem::is_regular_file(candidate, ignored))
            continue;
        if (texture.loadFromFile(candidate))
            return true;
    }
    return false;
}

} // namespace

CameraFeed::CameraFeed():Instrument("CameraFeed", 0), location_("Unknown"){}

CameraFeed::CameraFeed(std::string name, int id, std::string location):Instrument(std::move(name), id), location_(std::move(location)){}

std::string CameraFeed::getTypeName() const { return "Camera feed"; }

void CameraFeed::onClose(){
    if (!frames_.empty())
        currentFrame_ = (currentFrame_ + 1) % frames_.size();
}

void CameraFeed::addFrame(const std::string& imagePath){
    auto texture = std::make_unique<sf::Texture>();
    [[maybe_unused]] const bool loaded = loadTexture(*texture, imagePath);

    frames_.push_back(imagePath);
    textures_.push_back(std::move(texture));
}

bool CameraFeed::removeFrame(std::size_t index){
    if (index >= frames_.size())
        return false;

    frames_.erase(frames_.begin() + static_cast<std::ptrdiff_t>(index));
    textures_.erase(textures_.begin() + static_cast<std::ptrdiff_t>(index));
    if (currentFrame_ >= frames_.size())
        currentFrame_ = 0;
    return true;
}

int CameraFeed::findFrame(const std::string& imagePath) const{
    for (std::size_t i = 0; i < frames_.size(); ++i) {
        if (frames_[i] == imagePath)
            return static_cast<int>(i);
    }
    return -1;
}

void CameraFeed::draw(sf::RenderTarget& target, const sf::FloatRect& area) const{
    if (area.size.x <= 0.f || area.size.y <= 0.f)
        return;

    sf::RectangleShape background(area.size);
    background.setPosition(area.position);
    background.setFillColor(sf::Color(8, 10, 16));
    target.draw(background);

    const bool hasFrame = !frames_.empty();
    const sf::Texture* texture = hasFrame ? textures_[currentFrame_].get() : nullptr;
    const bool hasImage =
        texture != nullptr && texture->getSize().x > 0 && texture->getSize().y > 0;

    if (hasImage) {
        // fit the whole photo inside the window without stretching it.
        const float width = static_cast<float>(texture->getSize().x);
        const float height = static_cast<float>(texture->getSize().y);
        const float scale = std::min(area.size.x / width, area.size.y / height);

        sf::Sprite image(*texture);
        image.setScale({scale, scale});
        image.setPosition({area.position.x + (area.size.x - width * scale) / 2.f,
                           area.position.y + (area.size.y - height * scale) / 2.f});
        target.draw(image);
    } else {
        sf::Text message(defaultFont(), hasFrame ? "NO SIGNAL" : "NO FRAMES", 20);
        message.setFillColor(sf::Color(210, 210, 210));
        const sf::FloatRect bounds = message.getLocalBounds();
        message.setPosition(
            {area.position.x + (area.size.x - bounds.size.x) / 2.f - bounds.position.x,
             area.position.y + (area.size.y - bounds.size.y) / 2.f - bounds.position.y});
        target.draw(message);
    }

    // HUD strip to keep the caption and REC readable
    sf::RectangleShape hud({area.size.x, 34.f});
    hud.setPosition(area.position);
    hud.setFillColor(sf::Color(0, 0, 0, 140));
    target.draw(hud);

    const std::string frameText = hasFrame
        ? frames_[currentFrame_] + " (" + std::to_string(currentFrame_ + 1) + "/" +
          std::to_string(frames_.size()) + ")"
        : "no frames";

    sf::Text caption(defaultFont(),"CAM " + std::to_string(getId()) + " - " + location_ + " - " + frameText,14);
    caption.setFillColor(sf::Color(240, 240, 240));
    caption.setPosition({area.position.x + 14.f, area.position.y + 9.f});
    target.draw(caption);

    sf::CircleShape dot(5.f);
    dot.setFillColor(sf::Color(220, 40, 40));
    dot.setPosition({area.position.x + area.size.x - 70.f, area.position.y + 15.f});
    target.draw(dot);

    sf::Text rec(defaultFont(), "REC", 14);
    rec.setFillColor(sf::Color(240, 240, 240));
    rec.setPosition({area.position.x + area.size.x - 58.f, area.position.y + 9.f});
    target.draw(rec);
}
