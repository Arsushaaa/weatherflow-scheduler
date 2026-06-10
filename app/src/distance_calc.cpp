#include <cmath>

#include "distance_calc.hpp"
#include "structs.hpp"

constexpr double kEarthRadiusKm = 6371.0;

static double ToRadians(double degrees) {
   return degrees * 3.14159265358979323846 / 180.0;
}

double DistanceKm(const Coordinates& a, const Coordinates& b) {
   double lat1 = ToRadians(a.latitude);
   double lat2 = ToRadians(b.latitude);

   double d_lat = ToRadians(b.latitude - a.latitude);
   double d_lon = ToRadians(b.longitude - a.longitude);

   const double sin_lat = std::sin(d_lat / 2.0);
   const double sin_lon = std::sin(d_lon / 2.0);

   const double h = sin_lat * sin_lat +
         std::cos(lat1) * std::cos(lat2) * sin_lon * sin_lon;

   const double c = 2.0 * std::atan2(std::sqrt(h), std::sqrt(1.0 - h));

   return kEarthRadiusKm * c;
}