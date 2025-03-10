#pragma once
#include <cmath>
#include <iostream>

#include "position.h"

static constexpr double kWgs84EarthSemiMaj = 6378137.0;    // Earth's semi-major axis (meters)
static constexpr double kWgs84Flattening = 1 / 298.257223563;  // Flattening of the ellipsoid
static constexpr double kWgsEarthSemiMin = kWgs84EarthSemiMaj * (1 - kWgs84Flattening);
const double kWgs84ESquared = (std::pow(kWgs84EarthSemiMaj, 2) - std::pow(kWgsEarthSemiMin, 2)) / std::pow(kWgsEarthSemiMin, 2);// Square of the eccentricity

// Convert degrees to radians
inline double degToRad(const double deg) {
    return deg * M_PI / 180.0;
}



// Calculate the NED distance components between two LLH points
inline NorthEastDown LLHToNED(const PositionLLH& pos1, const PositionLLH& pos2) {
    // Convert latitude and longitude from degrees to radians
    const auto lat1 = degToRad(pos1.latitude);
    const auto lon1 = degToRad(pos1.longitude);
    const auto lat2 = degToRad(pos2.latitude);
    const auto lon2 = degToRad(pos2.longitude);

    // Compute prime vertical radius of curvature
    double sinLat1 = sin(lat1);
    double cosLat1 = cos(lat1);
    double N1 = kWgs84EarthSemiMaj / sqrt(1.0 - kWgs84ESquared * sinLat1 * sinLat1);  // Radius of curvature in the prime vertical

    // Compute the difference in coordinates
    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;
    double dHeight = pos2.ellipsoidHeight - pos1.ellipsoidHeight;

    // NED components
    NorthEastDown ned;
    ned.north = dLat * (kWgs84EarthSemiMaj * (1 - kWgs84ESquared)) / pow(1 - kWgs84ESquared * sinLat1 * sinLat1, 1.5);
    ned.east = dLon * N1 * cosLat1;
    ned.down = -dHeight;
    ned.horizontal = std::sqrt(std::pow(ned.north, 2) + std::pow(ned.east, 2));
    ned.spherical = std::sqrt(std::pow(ned.north, 2) + std::pow(ned.east, 2) + std::pow(ned.down, 2));



    return ned; 
}