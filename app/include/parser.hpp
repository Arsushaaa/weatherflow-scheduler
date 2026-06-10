#pragma once

#include <cpr/cpr.h>
#include <expected>
#include <string>
#include <vector>

#include "structs.hpp"

class Parser {
public:
   static std::expected<Coordinates, std::string> ResponseLocation(const cpr::Response& r);
   static std::expected<WeatherInfo, std::string> ResponseWeather(const cpr::Response& r);
   
   static std::expected<std::vector<Place>, std::string> 
   ResponsePlaces(const cpr::Response& r, const Coordinates& user_coords);
};