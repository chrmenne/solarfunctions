/*
 * solarfunctions: precise solar calculations
 * Copyright (c) 2024 Christian Menne
 * https://github.com/chrmenne/solarfunctions
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "Arduino.h"
#include "solarfunctions.h"

#define VERSION 1.0

// Various constants used in the formulae. All defined as double values as not to break arithmetic type conversion
constexpr double MEAN_RADIUS_VECTOR_CORRECTION = 1.000001018d;
constexpr double SOLAR_ANGULAR_DIAMETER_RAD = 0.0145380805d;
constexpr double SECONDS_PER_DAY = 86400.0;
constexpr double MINUTES_PER_DAY = 1440.0;
constexpr double SECONDS_PER_HOUR = 3600.0d;
constexpr double MINUTES_PER_HOUR = 60.0d;
constexpr double SECONDS_PER_MINUTE = 60.0d;
constexpr double MINUTES_PER_DEGREE_LONGITUDE = 4.0d; // 1 degree of Earth rotation per 4 Minutes
constexpr double DEGREES_LONGITUDE_PER_HOUR = 15.0d; // 15 degrees of Earth rotation per hour
constexpr double UNIX_EPOCH_JD = 2440587.5d; // 1970-01-01 12:00
constexpr double Y2K_JULIAN_DATE = 2451545.0d; // 2000-01-01 12:00
constexpr double SOLAR_ANOMALY_AT_EPOCH = 357.5291092d;
constexpr double SOLAR_ANOMALY_RATE = 35999.05029d;
constexpr double SOLAR_ANOMALY_CORRECTION = -0.0001537d;
constexpr double MEAN_LONGITUDE_SUN_AT_EPOCH = 280.46646d;
constexpr double MEAN_LONGITUDE_SUN_RATE = 36000.76983d;
constexpr double MEAN_LONGITUDE_SUN_CORRECTION = 0.0003032d;
constexpr double APPARENT_LONGITUDE_CORRECTION_1 = 0.00569d;
constexpr double APPARENT_LONGITUDE_CORRECTION_2 = 0.00478d;
constexpr double OBLIQUITY_CORRECTION_FACTOR = 0.00256d;
constexpr double MEAN_OBLIQUITY_AT_EPOCH = 23.439292d;
constexpr double OBLIQUITY_RATE1 = 0.013004167d;
constexpr double OBLIQUITY_RATE2 = 0.00000016389d;
constexpr double OBLIQUITY_RATE3 = 0.0000005036d;
constexpr double OMEGA_BASE_VALUE = 125.04d;
constexpr double OMEGA_RATE = -1934.136d;
constexpr double ECCENTRICITY_BASE = 0.016708634d;
constexpr double ECCENTRICITY_COEFF1 = 0.000042037d;
constexpr double ECCENTRICITY_COEFF2 = 0.0000001267d;
constexpr double SOLAR_EQ_CENTER_COEFF1 = 1.914602d;
constexpr double SOLAR_EQ_CENTER_COEFF2 = 0.004817d;
constexpr double SOLAR_EQ_CENTER_COEFF3 = 0.000014d;
constexpr double SOLAR_EQ_CENTER_COEFF4 = 0.019993d;
constexpr double SOLAR_EQ_CENTER_COEFF5 = 0.000101d;
constexpr double SOLAR_EQ_CENTER_COEFF6 = 0.000289d;

// Convert degrees to radians by multiplying with π/180.
double degToRad(double degrees) {
  return degrees * PI / 180;
}

// Convert radians to degrees by multiplying with 180/π.
double radToDeg(double radians) {
  return radians * 180 / PI;
}

// Normalize angle in degrees to the [-180, 180) range.
double normalizeDegrees180(double angle) {
  double degrees = normalizeDegrees360(angle);
  if (degrees > 180.0) {
    degrees -= 360.0;
  }
  return degrees;
}

// Normalize angle in degrees to the [0, 360) range.
double normalizeDegrees360(double angle) {
  return fmod(angle + 360.0, 360.0);
}

// Normalize angle in radians to the [-π, π) range.
double normalizeRadiansPi(double angle) {
  double radians = normalizeRadians2Pi(angle);
  if (radians > PI) {
    radians -= 2 * PI;
  }
  return radians;
}

// Normalize angle in radians to the [0, 2π) range.
double normalizeRadians2Pi(double angle) {
  return fmod(angle + 2 * PI, 2 * PI);
}

// Convert a Unix timestamp to Julian Date.
double calculateJulianDate(long timestamp_utc) {
  return (double) timestamp_utc / (double) SECONDS_PER_DAY + UNIX_EPOCH_JD;
}

// Calculate Julian Century from a given UTC time, with 2000-01-01 noon as starting point.
double calculateJulianCenturyNumber(long timestamp_utc) {
  return (calculateJulianDate(timestamp_utc) - Y2K_JULIAN_DATE) / 36525.0d;
}

// Earth's orbital eccentricity on a given Julian Century.
double calculateEccentricity(double julianCenturyNumber) {
  return ECCENTRICITY_BASE - julianCenturyNumber * (ECCENTRICITY_COEFF1 + ECCENTRICITY_COEFF2 * julianCenturyNumber);
}

// Mean obliquity (tilt) of the ecliptic in degrees.
double calculateObliquity(double julianCenturyNumber) {
  return MEAN_OBLIQUITY_AT_EPOCH - julianCenturyNumber * (OBLIQUITY_RATE1 + julianCenturyNumber * (OBLIQUITY_RATE2 - OBLIQUITY_RATE3 * julianCenturyNumber));
}

// Correct the mean obliquity with the longitude of the ascending lunar node.
double calculateCorrectedObliquity(double julianCenturyNumber) {
  return calculateCorrectedObliquity(calculateObliquity(julianCenturyNumber), calculateOmega(julianCenturyNumber));
}

// Correct the mean obliquity with longitude of the ascending lunar node (based on a pre-calculated Omega).
double calculateCorrectedObliquity(double obliquity, double omega) {
  return obliquity + OBLIQUITY_CORRECTION_FACTOR * cos(degToRad(omega));
}

// Longitude of the ascending lunar node.
double calculateOmega(double julianCenturyNumber) {
  return normalizeDegrees360(OMEGA_BASE_VALUE + OMEGA_RATE * julianCenturyNumber);
}

// Nutation in longitude for orbital corrections.
double calculateNutation(double julianCenturyNumber) {
  double omega = calculateOmega(julianCenturyNumber);
  double L_moon = OMEGA_BASE_VALUE + OMEGA_RATE * julianCenturyNumber;
  L_moon = normalizeDegrees360(L_moon);
  double L_sun = calculateSunMeanLongitude(julianCenturyNumber);
  // apply nutation formula
  double deltaPsi = -17.20 * sin(degToRad(omega))
                    - 1.32 * sin(2 * degToRad(L_sun))
                    + 0.23 * sin(2 * degToRad(L_moon))
                    + 0.21 * sin(2 * degToRad(omega));
  return deltaPsi / 3600.0;  // Convert arcseconds to degrees.
}

// The Sun's mean anomaly.
double calculateSunMeanAnomaly(double julianCenturyNumber) {
  double M = SOLAR_ANOMALY_AT_EPOCH + julianCenturyNumber * (SOLAR_ANOMALY_RATE - SOLAR_ANOMALY_CORRECTION * julianCenturyNumber);
  return normalizeDegrees360(M);
}

// Calculate the Sun's true anomaly by adding the equation of center.
double calculateSunTrueAnomaly(double julianCenturyNumber) {
  double meanAnomaly = calculateSunMeanAnomaly(julianCenturyNumber);
  return meanAnomaly + calculateSolarEquationOfCenter(julianCenturyNumber, meanAnomaly);
}

// The solar equation of center in degrees, adjusting the Sun's mean anomaly to account for elliptical orbit effects.
double calculateSolarEquationOfCenter(double julianCenturyNumber, double meanAnomaly) {
  return (SOLAR_EQ_CENTER_COEFF1 
		- julianCenturyNumber * (SOLAR_EQ_CENTER_COEFF2 + SOLAR_EQ_CENTER_COEFF3 * julianCenturyNumber)) * sin(degToRad(meanAnomaly)) 
		+ (SOLAR_EQ_CENTER_COEFF4 - SOLAR_EQ_CENTER_COEFF5 * julianCenturyNumber) * sin(degToRad(2 * meanAnomaly)) 
		+ SOLAR_EQ_CENTER_COEFF6 * sin(degToRad(3 * meanAnomaly));
}

// The Sun’s mean longitude.
double calculateSunMeanLongitude(double julianCenturyNumber) {
  double meanLongitude = MEAN_LONGITUDE_SUN_AT_EPOCH + julianCenturyNumber * (MEAN_LONGITUDE_SUN_RATE + julianCenturyNumber * MEAN_LONGITUDE_SUN_CORRECTION);
  return normalizeDegrees360(meanLongitude);
}

// Calculate the Sun's true longitude by adding the equation of center.
double calculateSunTrueLongitude(double julianCenturyNumber) {
  return calculateSunMeanLongitude(julianCenturyNumber) + calculateSolarEquationOfCenter(julianCenturyNumber, calculateSunMeanAnomaly(julianCenturyNumber));
}

// Calculate the Sun's apparent longitude, considering nutation effects.
double calculateSunApparentLongitude(double trueLongitude, double julianCenturyNumber) {
  return trueLongitude - APPARENT_LONGITUDE_CORRECTION_1 - APPARENT_LONGITUDE_CORRECTION_2 * sin(degToRad(calculateOmega(julianCenturyNumber)));
}

// Right Ascension for a given ecliptic latitude (lambda) and obliquity (epsilon).
double calculateRightAscension(double lambda, double epsilon) {
  double alpha = radToDeg(atan2(cos(degToRad(epsilon)) * sin(degToRad(lambda)), cos(degToRad(lambda))));
  return normalizeDegrees180(alpha);
}

// The Sun's declination in degrees based on the Julian century, which is the angular distance north or south of the celestial equator.
double calculateDeclination(double julianCenturyNumber) {
  double trueLongitude = calculateSunTrueLongitude(julianCenturyNumber);
  double lambda = calculateSunApparentLongitude(trueLongitude, julianCenturyNumber);
  double epsilon = calculateCorrectedObliquity(julianCenturyNumber);
  return radToDeg(asin(sin(degToRad(epsilon)) * sin(degToRad(lambda))));
}

// Equation of time in minutes.
double calculateEquationOfTime(double julianCenturyNumber) {
  double L0 = calculateSunMeanLongitude(julianCenturyNumber);
  double L0_rad = degToRad(L0);
  double M = calculateSunMeanAnomaly(julianCenturyNumber);
  double e = calculateEccentricity(julianCenturyNumber);
  double epsilon_corr = calculateCorrectedObliquity(julianCenturyNumber);
  double var_y = pow(tan(degToRad(epsilon_corr / 2)), 2);
  double M_rad = degToRad(M);
  // apply Equation of Time formula
  return 4 * radToDeg(var_y * sin(2 * L0_rad) - 2 * e * sin(M_rad) + 4 * e * var_y * sin(M_rad) * cos(2 * L0_rad) - 0.5 * var_y * var_y * sin(4 * L0_rad) - 1.25 * e * e * sin(2 * M_rad));
}

// Convert UTC time to local solar time, considering longitude and equation of time.
double calculateLocalSolarTime(long timestamp_utc, double equationOfTime, double longitude) {
  long secondsOfTheDay = timestamp_utc % (int) SECONDS_PER_DAY; // type conversion necessary for modulo operation
  double totalMinutes_utc = secondsOfTheDay / (double) SECONDS_PER_MINUTE;
  double timeOffset = longitude * MINUTES_PER_DEGREE_LONGITUDE;
  double localSolarTime = totalMinutes_utc + timeOffset + equationOfTime;
  // normalize to [0, 1440) if the calculated time is negative or exceeds 24 hours (1440 minutes).
  if (localSolarTime < 0) {
    localSolarTime += MINUTES_PER_DAY;
  } else if (localSolarTime >= MINUTES_PER_DAY) {
    localSolarTime -= MINUTES_PER_DAY;
  }
  return localSolarTime / SECONDS_PER_MINUTE;
}

// Local solar noon based on longitude and equation of time.
double calculateLocalSolarNoon(double timezone_offset, double equationOfTime, double longitude) {
  double solarNoonMinutes = (720 - (MINUTES_PER_DEGREE_LONGITUDE * longitude) - equationOfTime + (timezone_offset * MINUTES_PER_HOUR));
  return solarNoonMinutes / MINUTES_PER_DAY;
}

// The hour angle of the Sun in degrees from the local solar time.
double calculateHourAngle(double localSolarTime) {
  return normalizeDegrees180((localSolarTime - 12) * 15.0);
}

// The hour angle at sunrise based on declination and latitude, returning the angle in degrees.
double calculateSunriseHourAngle(double declination, double latitude) {
  double latitude_rad = degToRad(latitude);
  double declination_rad = degToRad(declination);
  double cosH0 = (-SOLAR_ANGULAR_DIAMETER_RAD - sin(latitude_rad) * sin(declination_rad)) / (cos(latitude_rad) * cos(declination_rad));
  double h0;
  // limit h0 to -180 or 0 degrees in cases the sun never sets or rises
  if (cosH0 <= -1.0) {
    h0 = -180;  // polar day
  } else if (cosH0 >= 1.0) {
    h0 = 0;  // polar night
  } else {
    h0 = -radToDeg(acos(cosH0)); // sunrise hour angles are always negative (eastward).
  }
  return h0;
}

// The hour angle at sunset based on declination and latitude, returning the angle in degrees.
double calculateSunsetHourAngle(double declination, double latitude) {
  return -calculateSunriseHourAngle(declination, latitude);
}

// The time of sunrise or sunset as a fraction of a day (24 hours) based on solar noon and the hour angle at sunrise/sunset.
double calculateSunriseSunsetTime(double solarNoon, double hourAngle_h0) {
  double time;
  if (abs(hourAngle_h0) >= 180 || hourAngle_h0 == 0) {
    time = INVALID_VALUE;
  } else {
    time = solarNoon + (hourAngle_h0 * MINUTES_PER_DEGREE_LONGITUDE / MINUTES_PER_DAY);
  }
  return time;
}

// The duration of daylight in minutes based on the hour angle at sunrise or sunset.
double calculateDaylightMinutes(double sunriseSunsetHourAngle) {
  double daylightMinutes;
  double angle = abs(sunriseSunsetHourAngle);
  if (angle >= 180) {
    daylightMinutes = MINUTES_PER_DAY;  // polar day
  } else if (angle <= 0.0) {
    daylightMinutes = 0;  // polar night
  } else {
    daylightMinutes = 2 * MINUTES_PER_HOUR * abs(sunriseSunsetHourAngle) / DEGREES_LONGITUDE_PER_HOUR;
  }
  return daylightMinutes;
}

// The solar elevation angle given hour angle, declination, and latitude.
double calculateSolarElevation(double hourAngle, double declination, double latitude) {
  double declination_rad = degToRad(declination);
  double latitude_rad = degToRad(latitude);
  double hourAngle_rad = degToRad(hourAngle);
  // apply elevation formula
  double elevation_rad = asin(sin(latitude_rad) * sin(declination_rad) + cos(latitude_rad) * cos(declination_rad) * cos(hourAngle_rad));
  return radToDeg(elevation_rad);
}

// Correct solar elevation for atmospheric refraction.
double calculateCorrectedSolarElevation(double elevation) {
  return elevation + calculateApproximateAtmosphericRefraction(elevation);
}

// Correct solar elevation for atmospheric refraction, given an already known refraction.
double calculateCorrectedSolarElevation(double elevation, double refraction) {
  return elevation + refraction;
}

// The solar azimuth angle based on latitude, declination, and hour angle.
double calculateSolarAzimuth(double hourAngle, double declination, double elevation, double latitude) {
  double azimuth;
  if (abs(latitude) >= 90) {
    if (elevation > 0) {
      azimuth = (latitude > 0) ? 180 : 0;  // Polar day: 180° at North Pole, 0° at South Pole
    } else {
      azimuth = INVALID_VALUE;  // Polar night
    }
  } else {
	double latitude_rad = degToRad(latitude);
	double declination_rad = degToRad(declination);
	double zenith_rad = degToRad(calculateSolarZenithAngle(elevation));
	double cosAzimuth = (sin(latitude_rad) * cos(zenith_rad) - sin(declination_rad)) / (cos(latitude_rad) * sin(zenith_rad));
	// Limit cos to values of [-1, 1]
	if (cosAzimuth < -1.0) {
		cosAzimuth = -1.0;
	} else if (cosAzimuth > 1.0) {
		cosAzimuth = 1.0;
	} 
	azimuth = radToDeg(acos(cosAzimuth));
	
	// normalize azimuth based on the hour angle
	if (hourAngle > 0) {
		azimuth = fmod(azimuth + 180.0, 360.0);
	} else {
		azimuth = fmod(540.0 - azimuth, 360.0);
	}
  }
  return azimuth;
}

// Solar zenith angle as 90° minus the solar elevation.
double calculateSolarZenithAngle(double trueelevation) {
  return 90 - trueelevation;
}

// The Sun's radius vector in AU based on Julian Century.
double calculateSunRadiusVector(double julianCenturyNumber) {
  return calculateSunRadiusVector(calculateEccentricity(julianCenturyNumber), calculateSunTrueAnomaly(julianCenturyNumber));
}

// The Sun's radius vector in AU based on orbital eccentricity and true anomaly.
double calculateSunRadiusVector(double eccentricity, double trueAnomaly) {
  double trueAnomaly_rad = degToRad(trueAnomaly);
  return (MEAN_RADIUS_VECTOR_CORRECTION * (1 - eccentricity * eccentricity)) / (1 + eccentricity * cos(trueAnomaly_rad));
}

// Estimates the approximate atmospheric refraction in degrees based on the elevation angle,
// adjusting apparent solar position near the horizon.
double calculateApproximateAtmosphericRefraction(double elevation) {
  double elevation_rad = degToRad(elevation);
  double refraction = 0.0;
  // different refraction formulae for different elevation ranges - refraction gets smaller with increasing elevation.
  if (elevation > 85.0) {
    refraction = 0.0;
  } else if (elevation > 5.0) {
    refraction = 58.1d / tan(elevation_rad) - 0.07d / pow(tan(elevation_rad), 3) + 0.000086d / pow(tan(elevation_rad), 5);
  } else if (elevation > -0.575d) {
    refraction = 1735.0d + elevation * (-518.2d + elevation * (103.4d + elevation * (-12.79d + elevation * 0.711d)));
  } else {
    refraction = -20.772d / tan(elevation_rad);
  }
  return refraction /= 3600.0;  // arc seconds to degrees
}
