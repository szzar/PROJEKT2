#pragma once
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>

#include<vector>
#include<utility>
#include<cstddef>
#include<memory>

#include "Ionosphere.h"

class Instrument{
    private:
        std::string name_ = "Unknown instrument";
        int id_ = 0;

    public:
        Instrument();
        Instrument(std::string name, int id);
        virtual ~Instrument() = default;

        int getId() const;
        void setName(const std::string& name);
        void setId(int id);
};

class Ionosonde: public Instrument{
    private:
        ParabolicLayer layer_;
        double startFreqMHz = 1.0;
        double endFreqMHz_ = 7.0;
        int sampleCount_ =240;

    public:
        Ionosonde();
        Ionosonde(std::string name, int id, const ParabolicLayer& layer, double startFreqMHz, double endFreqMHz, int sampleCount);
};

struct WeatherReading{
    double hour;
    double temperature;
};

class weatherStation: public Instrument{
    private:
        std::string location_;
        std::vector<WeatherReading> readings_;

    public:
        WeatherStation();
        WeatherStation(std::string name, int id, std::string location);

        std::string getTypeName() const override;
        void draw(sf::RenderTarget& target, const sf::FloatRect& area) const override;
        void addReading(double hour, double temperature);
        bool removeReading(std::size_t index);
        bool setReadingTemperature(std::size_t index, double temperature);
        int findReading(double hour) const;
        const WeatherReading& getReading(std::size_t index) const;

        double getMinTemp() const;
        double getMaxTemp() const;
        const std::string& getLocation() const;
        void setLocation(const std::string& location);
};

class CameraFeed: public Instrument{
    private:
        std::string location_;
        std::vector<std::string>> frames_;
        std::vector<std::unique_ptr<sf::Texture>> textures_;
        std::size_t currentFrame_ = 0;

    public:
        CameraFeed();
        CameraFeed(std::string name, int id, std::string location);

        std::string getTypeName(); const override;
        void draw(sf::RenderTarget& target, const sf::floatRect& area) const override;
        void onClose() override;
        void addFrame(const std::string& imagePath);

}
