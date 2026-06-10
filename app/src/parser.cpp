#include <algorithm>
#include <expected>
#include <string> 
#include <nlohmann/json.hpp>
#include <optional>
#include <utility>

#include "structs.hpp"
#include "parser.hpp"
#include "distance_calc.hpp"

using json = nlohmann::json;

std::expected<Coordinates, std::string> Parser::ResponseLocation(const cpr::Response& r) {
   json j = json::parse(r.text, nullptr, false);

   if (j.is_discarded()) {
      return std::unexpected("json error");
   }

   if (!j.contains("lat") || !j.contains("lon")) {
      return std::unexpected("no lat or lon in json");
   }

   Coordinates cords;

   cords.latitude = std::stod(j["lat"].get<std::string>());
   cords.longitude = std::stod(j["lon"].get<std::string>());

   return cords;
}

static WeatherType ClassifyWeatherCode(int code) {
   switch (code) {
      case 0: case 1: case 2:
         return WeatherType::kGood;
      
      case 3: case 45: case 48:
         return WeatherType::kNeutral;

      case 51: case 53: case 55:
      case 56: case 57: case 61: 
      case 63: case 65: case 66: 
      case 67: case 80: case 81: 
      case 82: 
         return WeatherType::kRainy;

      case 71: case 73: case 75:
      case 77: case 85: case 86:
         return WeatherType::kSnowy;

      case 95: case 96: case 99:
         return WeatherType::kDangerous;

      default: 
         return WeatherType::kNeutral;
   }
}

std::expected<WeatherInfo, std::string> Parser::ResponseWeather(const cpr::Response& r) {
   json j = json::parse(r.text, nullptr, false);

   if (j.is_discarded()) {
      return std::unexpected("json error");
   }

   if (!j.contains("current") || !j["current"].is_object()) {
      return std::unexpected("no current in weather json");
   }

   const auto& cur = j["current"];

   if (!cur.contains("temperature_2m") || 
       !cur.contains("precipitation") ||
       !cur.contains("precipitation_probability") || 
       !cur.contains("weather_code") || 
       !cur.contains("wind_speed_10m")) {

      return std::unexpected("weather json is not complete");
   }
   
   float temp = cur.value("temperature_2m", 0);
   float wind_speed = cur.value("wind_speed_10m", 0);
   WeatherType type = ClassifyWeatherCode(cur.value("weather_code", 0));
   float prec = cur.value("precipitation", 0);
   float prec_prob = cur.value("precipitation_probability", 0);

   return WeatherInfo(type, temp, wind_speed,
                   prec_prob, prec);
}

static std::optional<std::pair<std::string, std::string>> GetPlaceTypeInfo(const nlohmann::json& tags) {
   if (!tags.contains("name") || !tags["name"].is_string()) {
      return std::nullopt;
   }

   if (tags.contains("amenity") && tags["amenity"].is_string()) {
      return std::pair(tags["amenity"].get<std::string>(), tags["name"].get<std::string>());
   }

   if (tags.contains("tourism") && tags["tourism"].is_string()) {
      return std::pair(tags["tourism"].get<std::string>(), tags["name"].get<std::string>());
   }

   if (tags.contains("leisure") && tags["leisure"].is_string()) {
      return std::pair(tags["leisure"].get<std::string>(), tags["name"].get<std::string>());
   }

   if (tags.contains("shop") && tags["shop"].is_string()) {
      return std::pair(tags["shop"].get<std::string>(), tags["name"].get<std::string>());
   }

   if (tags.contains("natural") && tags["natural"].is_string()) {
      return std::pair(tags["natural"].get<std::string>(), tags["name"].get<std::string>());
   }

   return std::nullopt;
}

std::expected<std::vector<Place>, std::string> 
Parser::ResponsePlaces(const cpr::Response& r, const Coordinates& user_coords) {
   json j = json::parse(r.text, nullptr, false);

   if (j.is_discarded()) {
      return std::unexpected("json error");
   }

   if (!j.contains("elements") || !j["elements"].is_array()) {
      return std::unexpected("no elements in json places");
   }

   std::vector<Place> places;

   for (const auto& element : j["elements"]) {
      Place place;

      if (!element.contains("type") || !element["type"].is_string()) {
         continue;
      }

      std::string type = element.value("type", "");

      if (type == "node") {
         if (!element.contains("lat") || !element.contains("lon")) {
            continue;   
         }

         place.coords.latitude = element["lat"].get<double>();
         place.coords.longitude = element["lon"].get<double>();

      }
      else if (type == "way" || type == "relation") {
         if (!element.contains("center") || !element["center"].is_object()) {
            continue;
         }
         const auto& center = element["center"];

         if (!center.contains("lat") || !center.contains("lon")) {
            continue;
         }

         place.coords.latitude = center.value("lat", 0.0);
         place.coords.longitude = center.value("lon", 0.0);
      }
      else {
         continue;
      }

      if (!element.contains("tags") || !element["tags"].is_object()) {
         continue;
      }

      const auto& tags = element["tags"];

      auto placee_type_info = GetPlaceTypeInfo(tags);

      if (!placee_type_info) {
         continue;
      }

      place.type = placee_type_info->first;
      place.name = placee_type_info->second;

      place.distance_km = DistanceKm(user_coords, place.coords);

      places.push_back(std::move(place));
   }

   std::sort(places.begin(), places.end(), 
   [](const Place& a, const Place& b){
      return a.distance_km < b.distance_km;
   });

   return places;
}