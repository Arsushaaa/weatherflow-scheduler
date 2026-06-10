#include <cpr/cpr.h>
#include <string>
#include <format>

#include "response_to_api.hpp"

cpr::Response Api::GetLocation() {
   return cpr::Get(
      cpr::Url("https://api.2ip.io/"),
      cpr::Timeout(7500)
   );
}

cpr::Response Api::GetWeather(const Coordinates& cords) {
   return cpr::Get(
      cpr::Url("https://api.open-meteo.com/v1/forecast"),
      cpr::Parameters{
         {"latitude", std::to_string(cords.latitude)},
         {"longitude", std::to_string(cords.longitude)},
         {"current", "temperature_2m,precipitation,precipitation_probability,weather_code,wind_speed_10m"},
         {"past_days", "0"},
         {"forecast_days", "1"}
      },
      cpr::Timeout(10000)
   );
}


static SearchPlan BuildSearchPlan(const WeatherInfo& weather) {
   bool has_precipitation = weather.precipitation > 0.0f;
   bool high_precipitation_chance = weather.precipitation_probability >= 50.0f;
   bool strong_wind = weather.wind_speed >= 10.0f;
   bool very_strong_wind = weather.wind_speed >= 15.0f;
   bool cold = weather.temperature <= 0.0f;
   bool very_cold = weather.temperature <= -10.0f;
   bool hot = weather.temperature >= 25.0f;

   SearchPlan plan;
   plan.limit = 10;

   if (weather.type == WeatherType::kDangerous || very_strong_wind || 
                                       weather.precipitation >= 5.0f) {

      plan.radius_meters = 800;
      plan.filters = {
         {"amenity", "cafe|restaurant|fast_food|cinema|theatre|library"},
         {"tourism", "museum|gallery"},
         {"shop", "mall"}
      };
      return plan;
   }

   if (weather.type == WeatherType::kRainy || has_precipitation || 
                                       high_precipitation_chance) {

      plan.radius_meters = has_precipitation ? 1000 : 1400;
      plan.filters = {
         {"amenity", "cafe|restaurant|cinema|theatre|library"},
         {"tourism", "museum|gallery"},
         {"shop", "mall"}
      };
      return plan;
   }

   if (weather.type == WeatherType::kSnowy || very_cold) {
      plan.radius_meters = 1200;
      plan.filters = {
         {"amenity", "cafe|restaurant|cinema|theatre"},
         {"tourism", "museum|gallery|attraction"}
      };
      return plan;
   }

   if (cold || strong_wind) {
      plan.radius_meters = 1500;
      plan.filters = {
         {"amenity", "cafe|restaurant|cinema|theatre"},
         {"tourism", "museum|gallery|attraction"}
      };
      return plan;
   }

   if (hot) {
      plan.radius_meters = 2500;
      plan.filters = {
         {"leisure", "park|garden"},
         {"natural", "beach|water"},
         {"tourism", "viewpoint|attraction"},
         {"amenity", "cafe|restaurant|ice_cream"}
      };
      return plan;
   }

   if (weather.type == WeatherType::kGood) {
      plan.radius_meters = 2500;
      plan.filters = {
         {"leisure", "park|garden"},
         {"tourism", "attraction|viewpoint|museum|gallery"},
         {"amenity", "cafe|restaurant"}
      };
      return plan;
   }

   plan.radius_meters = 1800;
   plan.filters = {
      {"amenity", "cafe|restaurant"},
      {"tourism", "museum|gallery|attraction|viewpoint"},
      {"leisure", "park|garden"}
   };

   return plan;
}


static std::string MakeOverpassQuery(const Coordinates& coords, 
                                    const WeatherInfo& weather) {

   SearchPlan plan = BuildSearchPlan(weather);

   std::string query; 
   query += "[out:json][timeout:25];\n";
   query += "(\n";

   for (const auto& filter : plan.filters) {
      query += std::format(
         "  nwr(around:{},{},{})[\"{}\"~\"{}\"];\n",
         plan.radius_meters,
         coords.latitude,
         coords.longitude,
         filter.key,
         filter.value
      );
   }

   query += ");\n";
   query += std::format("out center {};\n", plan.limit);

   return query;
}

cpr::Response Api::GetPlaces(const Coordinates &cords, const WeatherInfo& weather) {
   std::string query = MakeOverpassQuery(cords, weather);

   return cpr::Post(
      cpr::Url("https://overpass-api.de/api/interpreter"),
      cpr::Header{
         {"Accept", "application/json"},
         {"Content-Type", "application/x-www-form-urlencoded"},
         {"User-Agent", "labwork9-Arsushaaa (student weather places recommender)"}
      },
      cpr::Payload{{"data", query}},
      cpr::Timeout(15000)
   );
}



std::expected<void, std::string> Api::CheckResponse(const cpr::Response& r) {
   if (r.error.code != cpr::ErrorCode::OK) {
      return std::unexpected("network error: " + r.error.message);
   }

   if (r.status_code != 200) {
      return std::unexpected("http error: " + std::to_string(r.status_code));
   }

   return {};
}
