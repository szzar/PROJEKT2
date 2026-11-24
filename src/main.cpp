#include <SFML/Graphics.hpp>

#include <cmath>     // std::sin
#include <cstddef>   // std::size_t
#include <iostream>
#include <limits>    // std::numeric_limits
#include <optional>  // pollEvent returns std::optional<sf::Event>. a SFML thing
#include <string> 
#include <vector>

#include "Instrument.h"
#include "Ionosphere.h" 
//big ass namespace 
namespace{ constexpr double kPi = 3.14159265358979323846;
constexpr std::size_t kConsoleWidth = 52;
constexpr std::size_t kConsoleInner = kConsoleWidth - 4;

//drawing initial screen
void printBorder(){
    std::cout << std::string(kConsoleWidth, '*') << '\n';
}

void printRow(std::string text){
    if (text.size() > kConsoleInner)
        text.resize(kConsoleInner);
    std::cout << "* " << text
              << std::string(kConsoleInner - text.size(), ' ') << " *\n";
}

void printCentered(std::string text){
    if (text.size() > kConsoleInner)
        text.resize(kConsoleInner);

    const std::size_t left = (kConsoleInner - text.size()) / 2;
    const std::size_t right = kConsoleInner - text.size() - left;
    std::cout << "* " << std::string(left, ' ') << text
              << std::string(right, ' ') << " *\n";
}

void printBanner(std::size_t instrumentCount){
    printBorder();
    printRow("");
    printCentered("***  RESEARCH FACILITY SIMULATION  ***");
    printCentered(std::to_string(instrumentCount) + " instruments online");
    printRow("");
    printBorder();
}

void showInstrument(const Instrument& instrument, const std::string& title){
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

void printMenu(const std::vector<Instrument*>& instruments){
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

}
