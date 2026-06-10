#pragma once

#include <vector>
#include <string>
#include <expected>

#include "structs.hpp"

struct WeatherAndPlaces {
   WeatherInfo weather_info;
   std::vector<Place> places;
};

class MainLogic {
private:

public:
   static std::expected<Coordinates, std::string> GetLocation();
   static std::expected<WeatherAndPlaces, std::string> 
   GetWeatherAndPlaces(const Coordinates& coords);
};