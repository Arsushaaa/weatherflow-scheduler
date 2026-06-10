#include <expected>
#include <exception>

#include "main_logic.hpp"
#include "response_to_api.hpp"
#include "parser.hpp"
#include "TTaskScheduler.hpp"

template <typename T>
T Unwrap(std::expected<T, std::string> result) {
   if (!result.has_value()) {
      throw std::runtime_error(result.error());
   }

   return std::move(result.value());
}

void CheckApiResponse(const cpr::Response& response) {
   auto check = Api::CheckResponse(response);

   if (!check.has_value()) {
      throw std::runtime_error(check.error());
   }
}



std::expected<Coordinates, std::string> MainLogic::GetLocation() {
   try {
      ts::TTaskScheduler scheduler;

      auto coords_responce = scheduler.add([](){
         return Api::GetLocation();
      });

      auto coords = scheduler.add([](const cpr::Response& location){
            CheckApiResponse(location);
            return Unwrap(Parser::ResponseLocation(location));
         },
         coords_responce.getFutureResult<const cpr::Response&>()
      );

      return coords.getResultSync();
   }
   catch (const std::exception& e) {
      return std::unexpected("App error: " + std::string(e.what()));
   }
   catch (...) {
      return std::unexpected("Unkown app error");
   }
}



std::expected<WeatherAndPlaces, std::string> 
MainLogic::GetWeatherAndPlaces(const Coordinates& coords) {
   try {
      ts::TTaskScheduler scheduler;

      auto weather_responce = scheduler.add([](const Coordinates& coords){
            return Api::GetWeather(coords);
         },
         coords
      );

      auto weather = scheduler.add([](const cpr::Response& weather_resp){
            CheckApiResponse(weather_resp);
            return Unwrap(Parser::ResponseWeather(weather_resp));
         },
         weather_responce.getFutureResult<const cpr::Response&>()
      );

      auto places_responce = scheduler.add([](const Coordinates& coords, const WeatherInfo& weather){
            return Api::GetPlaces(coords, weather);
         }, 
         coords,
         weather.getFutureResult<const WeatherInfo&>()
      );

      auto places = scheduler.add([](const cpr::Response& places_resp, const Coordinates& coords){
            CheckApiResponse(places_resp);
            return Unwrap(Parser::ResponsePlaces(places_resp, coords));
         },
         places_responce.getFutureResult<const cpr::Response&>(),
         coords
      );

      scheduler.executeAll();

      return WeatherAndPlaces{weather.getResultSync(), places.getResultSync()};
   }
   catch (const std::exception& e) {
      return std::unexpected("App error: " + std::string(e.what()));
   }
   catch (...) {
      return std::unexpected("Unkown app error");
   }
}