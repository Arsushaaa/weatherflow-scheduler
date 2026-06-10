#pragma once

#include <string>
#include <vector>

enum class WeatherType {
   kGood,
   kNeutral,
   kRainy,
   kSnowy,
   kDangerous
};

struct Coordinates {
   double latitude;
   double longitude;
};

struct WeatherInfo {
   WeatherType type;
   float temperature;
   float wind_speed;
   float precipitation_probability;
   float precipitation;
};

struct OverpassFilter {
   std::string key;
   std::string value;
};

struct SearchPlan {
   int radius_meters;
   int limit;
   std::vector<OverpassFilter> filters;
};

struct Place {
   std::string name;
   std::string type;
   double distance_km;
   Coordinates coords;
   // либо distance либо FindDistance() 
};