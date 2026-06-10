#pragma once

#include <cpr/cpr.h>
#include <expected>
#include <string>

#include "structs.hpp"

class Api {
private:

public:
   static cpr::Response GetLocation();
   static cpr::Response GetWeather(const Coordinates& cords);
   static cpr::Response GetPlaces(const Coordinates& cords, const WeatherInfo& weather);
   static std::expected<void, std::string> CheckResponse(const cpr::Response& r);
};