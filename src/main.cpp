#include <SFML/Graphics.hpp>

#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <vector>

#include "Instrument.h"
#include "Ionosphere.h"

namespace {

constexpr double kPi = 3.14159265358979323846;

// Console box width in characters. Stars are the border because the project
// is sky-themed; the inner width leaves room for "* " and " *".
constexpr std::size_t kConsoleWidth = 52;

void printBorder()
{
    std::cout << std::string(kConsoleWidth, '*') << '\n';
}

void printRow(std::string text)
{
    const std::size_t inner = kConsoleWidth - 4;
    if (text.size() > inner)
        text.resize(inner);
    std::cout << "* " << text << std::string(inner - text.size(), ' ') << " *\n";
}

void printCentered(std::string text)
{
    const std::size_t inner = kConsoleWidth - 4;
    if (text.size() > inner)
        text.resize(inner);

    const std::size_t left = (inner - text.size()) / 2;
    const std::size_t right = inner - text.size() - left;
    std::cout << "* " << std::string(left, ' ') << text
              << std::string(right, ' ') << " *\n";
}

void printBanner(std::size_t instrumentCount)
{
    printBorder();
    printRow("");
    printCentered("***  RESEARCH FACILITY SIMULATION  ***");
    printCentered(std::to_string(instrumentCount) + " instruments online");
    printRow("");
    printBorder();
}

void showInstrument(const Instrument& instrument, const std::string& title)
{
    sf::RenderWindow window(sf::VideoMode({720u, 520u}), title);
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        const sf::Vector2u size = window.getSize();
        window.clear(sf::Color(255, 255, 250));
        instrument.draw(window,
                        sf::FloatRect({0.f, 0.f},
                                      {static_cast<float>(size.x),
                                       static_cast<float>(size.y)}));
        window.display();
    }
}

void printMenu(const std::vector<Instrument*>& instruments)
{
    std::cout << '\n';
    printBorder();
    printCentered("=== RESEARCH FACILITY CONTROL ===");
    printRow("");

    for (std::size_t i = 0; i < instruments.size(); ++i) {
        printRow(" " + std::to_string(i + 1) + ") " +
                 instruments[i]->getTypeName() + " - " +
                 instruments[i]->getName());
    }

    printRow(" 0) Quit");
    printBorder();
    std::cout << "Choice: ";
}

} // namespace

int main()
{
    // F2 layer: peak 300 km, half-thickness 100 km, critical 7 MHz.
    ParabolicLayer f2Layer(300.0, 100.0, 7.0);
    Ionosonde ionosonde("Ionosonde-01", 1, f2Layer, 1.0, 7.0, 240);

    WeatherStation weather;
    weather.setName("WeatherStation-01");
    weather.setId(2);
    weather.setLocation("North Ridge Site");

    CameraFeed camera("CameraFeed-01", 3, "North Gate");

    // Synthetic weather data
    for (int hour = 0; hour <= 24; ++hour) {
        const double temperature =
            23.0 + 5.5 * std::sin((hour - 9.0) * kPi / 12.0)
                 + 0.6 * std::sin(hour * 1.7);
        weather.addReading(hour, temperature);
    }

    const int closing = weather.findReading(24.0); // < removes redundant 24:00 data
    if (closing >= 0)
        weather.removeReading(static_cast<std::size_t>(closing));

    const int noon = weather.findReading(12.0);
    if (noon >= 0) {
        const auto index = static_cast<std::size_t>(noon);
        weather.setReadingTemperature(index, weather.getReading(index).temperatureC + 0.5);
    }

    camera.addFrame("gui/pic1.png");
    camera.addFrame("gui/pic2.png");
    camera.addFrame("gui/pic3.png");

    std::vector<Instrument*> instruments{&ionosonde, &weather, &camera};

    printBanner(instruments.size());
    std::cout << "F2 layer: hm=" << f2Layer.getPeakHeight()
              << " km, ym=" << f2Layer.getHalfThickness()
              << " km, foF2=" << f2Layer.getCriticalFrequency() << " MHz\n";

    while (true) {
        printMenu(instruments);

        int choice = -1;
        if (!(std::cin >> choice)) {
            if (std::cin.eof())
                break;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input.\n";
            continue;
        }

        if (choice == 0)
            break;

        if (choice < 1 || choice > static_cast<int>(instruments.size())) {
            std::cout << "No such instrument.\n";
            continue;
        }

        Instrument& picked = *instruments[static_cast<std::size_t>(choice) - 1];

        std::cout << "Opening " << picked.getTypeName()
                  << " \"" << picked.getName() << "\"...\n";

        showInstrument(picked, picked.getTypeName() + " - " + picked.getName());
        picked.onClose();
        std::cout << "Window closed.\n";
    }

    std::cout << "Shutting down.\n";
    return 0;
}
