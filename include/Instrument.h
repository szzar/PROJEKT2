#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

#include "Ionosphere.h"

// Abstract base class: every instrument has an identity (name + id) and knows

class Instrument
{
public:
    Instrument();
    Instrument(std::string name, int id);
    virtual ~Instrument() = default;

    const std::string& getName() const;
    int getId() const;
    void setName(const std::string& name);
    void setId(int id);

    virtual void onClose() {}

    virtual std::string getTypeName() const = 0;
    virtual void draw(sf::RenderTarget& target, const sf::FloatRect& area) const = 0;

private:
    std::string name_ = "Unnamed instrument";
    int id_ = 0;
};


// Ionosonde

class Ionosonde : public Instrument
{
public:
    Ionosonde();
    Ionosonde(std::string name, int id, const ParabolicLayer& layer,
              double startFreqMHz, double endFreqMHz, int sampleCount);

    std::string getTypeName() const override;
    void draw(sf::RenderTarget& target, const sf::FloatRect& area) const override;

    std::vector<std::pair<double, double>> computeIonogram() const;

private:
    ParabolicLayer layer_;
    double startFreqMHz_ = 1.0;
    double endFreqMHz_ = 7.0;
    int sampleCount_ = 240;
};




struct WeatherReading
{
    double hour;
    double temperatureC;
};


// WeatherStation vv  ^^ + Sample

class WeatherStation : public Instrument
{
public:
    WeatherStation();
    WeatherStation(std::string name, int id, std::string location);

    std::string getTypeName() const override;
    void draw(sf::RenderTarget& target, const sf::FloatRect& area) const override;

    void addReading(double hour, double temperatureC);
    bool removeReading(std::size_t index);
    bool setReadingTemperature(std::size_t index, double temperatureC);
    int findReading(double hour) const;             
    const WeatherReading& getReading(std::size_t index) const;

    double getMinTemperature() const;
    double getMaxTemperature() const;
    const std::string& getLocation() const;
    void setLocation(const std::string& location);

private:
    std::string location_;
    std::vector<WeatherReading> readings_;
};


// CameraFeed

class CameraFeed : public Instrument
{
public:
    CameraFeed();
    CameraFeed(std::string name, int id, std::string location);

    std::string getTypeName() const override;
    void draw(sf::RenderTarget& target, const sf::FloatRect& area) const override;
    void onClose() override;

    void addFrame(const std::string& imagePath);
    bool removeFrame(std::size_t index);
    int findFrame(const std::string& imagePath) const;

private:
    std::string location_;
    std::vector<std::string> frames_;                    
    std::vector<std::unique_ptr<sf::Texture>> textures_;  
    std::size_t currentFrame_ = 0;
};
