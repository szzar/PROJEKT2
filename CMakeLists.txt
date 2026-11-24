cmake_minimum_required(VERSION 3.20)

project(ResearchFacility LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG        3.1.0
)
FetchContent_MakeAvailable(SFML)

add_executable(research_facility
    src/main.cpp
    src/Instrument.cpp
    src/Ionosphere.cpp
    src/Plot.cpp
)

target_include_directories(research_facility PRIVATE include)

# SFML::Graphics pulls in Window and System automatically.
target_link_libraries(research_facility PRIVATE SFML::Graphics)
