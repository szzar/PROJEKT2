# Research Facility Simulation

A mock-up simulation of a research facility where the console is the control panel. Every instrument opens its own SFML window based on its gimmick (plot/images).
C++17, SFML 3.1 and CMake 3.20 required!

## Build and run

```bash
cmake -S . -B build
cmake --build build
./build/research_facility
```

Windows: `.\build\Release\research_facility.exe`.

Acquaint yourself with SFML documentation for [necessary dependencies](https://www.sfml-dev.org/tutorials/3.1/getting-started/cmake/#requirements) for your operating system.

## Example session

```
=== Research Facility Control ===
 1) Ionosonde - Ionosonde-01
 2) Weather station - WeatherStation-01
 3) Camera feed - CameraFeed-01
 0) Quit
Choice: 1
Opening Ionosonde "Ionosonde-01"...
[SFML window opens with the ionogram]
Window closed.
```

## Layout

```
CMakeLists.txt
include/   Instrument.h  Ionosphere.h  Plot.h
src/       main.cpp  Instrument.cpp  Ionosphere.cpp  Plot.cpp
gui/       pic1.png  pic2.png  terminus.ttf
```

## UML class diagram

```mermaid
classDiagram
    direction TB

    class Instrument {
        <<abstract>>
        -string name_
        -int id_
        +Instrument()
        +Instrument(string, int)
        +getName() const
        +getId() const
        +setName(string)
        +setId(int)
        +onClose()
        +getTypeName() const
        +draw(RenderTarget, FloatRect) const
    }

    class Ionosonde {
        -ParabolicLayer layer_
        -double startFreqMHz_
        -double endFreqMHz_
        -int sampleCount_
        +Ionosonde()
        +Ionosonde(string, int, ParabolicLayer, double, double, int)
        +getTypeName() const
        +draw(RenderTarget, FloatRect) const
        +computeIonogram() const
    }

    class WeatherStation {
        -string location_
        -vector readings_
        +WeatherStation()
        +WeatherStation(string, int, string)
        +getTypeName() const
        +draw(RenderTarget, FloatRect) const
        +addReading(double, double)
        +removeReading(size_t) bool
        +setReadingTemperature(size_t, double) bool
        +findReading(double) int
        +getReading(size_t) const
        +getMinTemperature() const
        +getMaxTemperature() const
        +getLocation() const
        +setLocation(string)
    }

    class CameraFeed {
        -string location_
        -vector frames_
        -vector textures_
        -size_t currentFrame_
        +CameraFeed()
        +CameraFeed(string, int, string)
        +getTypeName() const
        +draw(RenderTarget, FloatRect) const
        +onClose()
        +addFrame(string)
        +removeFrame(size_t) bool
        +findFrame(string) int
    }

    class ParabolicLayer {
        -double peakHeightKm_
        -double halfThicknessKm_
        -double criticalFreqMHz_
        +ParabolicLayer()
        +ParabolicLayer(double, double, double)
        +getPeakHeight() const
        +getHalfThickness() const
        +getCriticalFrequency() const
        +reflects(double) const
        +virtualHeight(double) const
    }

    class WeatherReading {
        +double hour
        +double temperatureC
    }

    class Plot {
        -FloatRect area_
        -double xMin_
        -double xMax_
        -double yMin_
        -double yMax_
        -string title_
        -string xLabel_
        -string yLabel_
        -int xTicks_
        -int yTicks_
        -vector series_
        +Plot(FloatRect, double, double, double, double, string, string, string)
        +setTickCount(int, int)
        +addSeries(vector, Color)
        +draw(RenderTarget) const
    }

    class Series {
        +vector points
        +Color color
    }

    Instrument <|-- Ionosonde
    Instrument <|-- WeatherStation
    Instrument <|-- CameraFeed
    Ionosonde *-- ParabolicLayer : composition
    WeatherStation *-- WeatherReading : composition
    Plot *-- Series : composition
    Ionosonde ..> Plot : creates
    WeatherStation ..> Plot : creates
```
