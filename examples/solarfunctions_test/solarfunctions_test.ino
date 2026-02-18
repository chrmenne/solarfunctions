/*
 * solarfunctions_test.ino
 * Test/example sketch for the solarfunctions lib
 * https://github.com/GitCoyote/solarfunctions
 *
 * Runs through a set of defined test-cases and prints deviations from the expected values.
 */

#include <solarfunctions.h>


void setup() {
  Serial.begin(115200);
  delay(3000);  // power-up safety delay
}

void loop() {

  // Test case info taken from the Excel file from https://gml.noaa.gov/grad/solcalc/calcdetails.html
  // Time stamps were calculated using https://www.pelock.com/products/epoch-unix-time-stamp-converter
  //   latitude,  longitude,    time,       TZ,  JCN,           Declination,     RightAsc,     RadiusVector, SunsetTime,      Azimuth,       SolarTime,       SolarNoon,      CorrElev,       LengthOfDay, Description
  // Quito, spring equinox 2024-03-20, 01:00/07:00/13:00 local time (UTC-5)
  test(0, -78.4678, 1710896400, -5, 0.24215606, 0.048230654, 0.111248903, 0.995917448, 0.767061184, 89.71348488, 38.7638959 / 60.0, 0.514747295, -80.30792179, 726.6640024, "Quito, spring equinox 2024-03-20 01:00 local time (UTC-5)");
  test(0, -78.4678, 1710918000, -5, 0.24216290, 0.14702914, 0.33914008, 0.9959874, 0.767009661, 89.85083414, 398.8380991 / 60.0, 0.514695764, 9.800096027, 726.6640219, "Quito, spring equinox 2024-03-20 07:00 local time (UTC-5)");
  test(0, -78.4678, 1710939600, -5, 0.24216975, 0.245811417, 0.567001248, 0.996057422, 0.766958066, 271.4544356, 758.9124152 / 60.0, 0.514644156, 80.27158859, 726.6640613, "Quito, spring equinox 2024-03-20 13:00 local time (UTC-5)");
  // Quito, fall equinox 2024-09-23, 01:00/07:00/13:00 local time (UTC-5)
  test(0, -78.4678, 1727053200, -5, 0.24727584, -0.28188216, -179.3497922, 1.003390741, 0.756601971, 91.21116461, 53.8252014 / 60.0, 0.504288055, -76.53942111, 726.6640807, "Quito, fall equinox 2024-09-23 01:00 local time (UTC-5)");
  test(0, -78.4678, 1727074800, -5, 0.24728268, -0.379220386, -179.1252446, 1.003320629, 0.756541166, 90.38996008, 413.9127945 / 60.0, 0.504227226, 13.5438534, 726.664146, "Quito, fall equinox 2024-09-23 07:00 local time (UTC-5)");
  test(0, -78.4678, 1727096400, -5, 0.24728953, -0.476566397, -178.9006606, 1.003250452, 0.756480461, 267.9593782, 774.0002513 / 60.0, 0.504166492, 76.49556112, 726.6642305, "Quito, fall equinox 2024-09-23 13:00 local time (UTC-5)");
  // Nuremberg, 2024-07-15, 04:06/14:00/22:54 local time (UTC+2/DST)
  test(49.734839, 11.156555, 1721016360, 2, 0.24535489, 21.46187447, 114.9304399, 1.016450315, 0.887717195, 38.45394729, 164.6076673 / 60.0, 0.556522453, -9.971896105, 953.8408549, "Nuremberg 2024-07-15 04:06 local time (UTC+2/DST)");
  test(49.734839, 11.156555, 1721052000, 2, 0.24536619, 21.39538562, 115.3470636, 1.016429856, 0.887454915, 198.5525646, 758.567444 / 60.0, 0.556550386, 60.66000295, 953.0050422, "Nuremberg 2024-07-15 14:00 local time (UTC+2/DST)");
  test(49.734839, 11.156555, 1721084040, 2, 0.24537634, 21.33473278, 115.721293, 1.01641078, 0.887214914, 325.1903798, 1292.532528 / 60.0, 0.556574634, -11.74516359, 952.244009, "Nuremberg 2024-07-15 22:54 local time (UTC+2/DST)");
  // Nuremberg, 2024-01-15, 08:24/13:00/16:36 local time (UTC+1)
  test(49.734839, 11.156555, 1705307040, 1, 0.24037805, -21.19812267, -63.45519523, 0.983614571, 0.695631064, 126.0263058, 479.4656921 / 60.0, 0.517037714, 1.713437273, 514.34885, "Nuremberg 2024-01-15 08:24 local time (UTC+1)");
  test(49.734839, 11.156555, 1705323600, 1, 0.24038330, -21.16329308, -63.2491759, 0.983625434, 0.695823468, 188.7095982, 755.3970968 / 60.0, 0.517085349, 18.71467248, 514.7657826, "Nuremberg 2024-01-15 13:00 local time (UTC+1)");
  test(49.734839, 11.156555, 1705336560, 1,  0.24038741, -21.13586177, -63.08801441, 0.983634064, 0.69597448, 236.0834078, 971.3436944 / 60.0, 0.517122434, 0.48578732, 515.0938899, "Nuremberg 2024-01-15 16:36 local time (UTC+1)");
  // Arctic Circle, summer solstice 2024-06-21, 00:00/12:00/18:00 local time (UTC+0)
  test(66.5622, 0, 1718928000, 0, 0.24469541, 23.43855495, 90.13761524, 1.016241671, INVALID_VALUE, 359.5834118, 1438.183783 / 60.0, 0.501261261, 0.483152831, 1440, "Arctic Circle, summer solstice 2024-06-21, 00:00 local time (UTC+0)");
  test(66.5622, 0, 1718971200, 0, 0.24470910, 23.43723796, 90.65763876, 1.01627395, INVALID_VALUE, 179.3541348, 718.0751405 / 60.0, 0.501336708, 46.88905947, 1440, "Arctic Circle, summer solstice 2024-06-21, 12:00 local time (UTC+0)");
  test(66.5622, 0, 1718992800, 0, 0.24471595, 23.43593364, 90.9176327, 1.016289653, INVALID_VALUE, 279.3416442, 1078.020889 / 60.0, 0.501374383, 21.63645706, 1440, "Arctic Circle, summer solstice 2024-06-21, 18:00 local time (UTC+0)");
  // Arctic Circle, winter solstice 2024-12-21, 00:00/12:00/18:00 local time (UTC+0)
  test(66.5622, 0, 1734739200, 0, 0.24970568, -23.43802069, -90.42731352, 0.983766103, 0.543729144, 0.646212104, 1.925876741 / 60.0, 0.498662586, -46.8693365, 129.7979624, "Arctic Circle, winter solstice 2024-12-21, 00:00 local time (UTC+0)");
  test(66.5622, 0, 1734782400, 0, 0.24971937, -23.43855019, -89.87235135, 0.983732575, 0.543889106, 180.3848909, 721.6780243 / 60.0, 0.498834705, 0.480822517, 129.7566742, "Arctic Circle, winter solstice 2024-12-21, 12:00 local time (UTC+0)");
  test(66.5622, 0, 1734804000, 0, 0.24972621, -23.43807926, -89.59485665, 0.983716277, 0.543987951, 260.562598, 1081.554049 / 60.0, 0.498920799, -21.54162994, 129.7933957, "Arctic Circle, winter solstice 2024-12-21, 18:00 local time (UTC+0)");
  // North Pole, summer solstice 2024-06-21, 12:00 local time (UTC+0) (polar day)
  test(90, 0, 1718971200, 0, 0.24470910, 23.43723796, 90.65763876, 1.01627395, INVALID_VALUE, 180, 718.0751405 / 60.0, 0.501336708, 23.47422922, 1440, "North Pole, summer solstice 2024-06-21, 12:00 local time (UTC+0) (polar day)");
  // North Pole, winter solstice 2024-12-21, 12:00 local time (UTC+0) (polar night)
  test(90, 0, 1734782400, 0, 0.24971937, -23.43855019, -89.87235135, 0.983732575, INVALID_VALUE, INVALID_VALUE, 721.6780243 / 60.0, 0.498834705, -23.42529106, 0, "North Pole, winter solstice 2024-12-21, 12:00 local time (UTC+0) (polar night)");
  // South Pole, summer solstice 2024-06-21, 12:00 local time (UTC+0) (polar night)
  test(-90, 0, 1718971200, 0, 0.24470910, 23.43723796, 90.65763876, 1.01627395, INVALID_VALUE, INVALID_VALUE, 718.0751405 / 60.0, 0.501336708, -23.42392801, 0, "South Pole, summer solstice 2024-06-21, 12:00 local time (UTC+0) (polar night)");
  // South Pole, winter solstice 2024-12-21, 12:00 local time (UTC+0) (polar day)
  test(-90, 0, 1734782400, 0, 0.24971937, -23.43855019, -89.87235135, 0.983732575, INVALID_VALUE, 0, 721.6780243 / 60.0, 0.498834705, 23.47553916, 1440, "South Pole, winter solstice 2024-12-21, 12:00 local time (UTC+0) (polar day)");

  // loop eternally
  Serial.println("");
  Serial.println("");
  delay(5000);
}

// Test function. To calculate the values expected, every function in the library needs to be used at least once. 
// Detailed output is only printed on mismatches.
boolean test(double location_latitude,
             double location_longitude,
             int64_t timestamp,
             int tz_offset, 
             double expectedJulianCenturyNumber,
             double expectedDeclination,
             double expectedRightAscension,
             double expectedSunRadiusVector,
             double expectedSunsetTime,
             double expectedAzimuth,
             double expectedLocalSolarTime,
             double expectedLocalSolarNoon,
             double expectedCorrectedSolarElevation,
             double expectedLengthOfDay,
             String description) {
  boolean isSuccessful = true;

  int64_t timestamp_utc = timestamp - (tz_offset * 3600);

  Serial.println("Test case: " + description);
  //Serial.println("\tLocal " + String(timestamp) + " " + String(hour(timestamp)) + ":" + String(minute(timestamp)) + " --> UTC: " + String(timestamp_utc) + " " + String(h) + ":" + String(m));
  
  // calculate Julian Century Number
  double julianCenturyNumber = calculateJulianCenturyNumber(timestamp_utc);
  isSuccessful &= evaluate("Julian century number", julianCenturyNumber, expectedJulianCenturyNumber);

  // calculate declination
  double declination = calculateDeclination(julianCenturyNumber);
  isSuccessful &= evaluate("Declination", declination, expectedDeclination);

  // calculate right ascension with true longitude, apparent longitude and corrected obliquity
  double trueLongitude = calculateSunTrueLongitude(julianCenturyNumber);
  double apparentLongitude = calculateSunApparentLongitude(trueLongitude, julianCenturyNumber);
  double correctedObliquity = calculateCorrectedObliquity(julianCenturyNumber);
  double rightAscension = calculateRightAscension(apparentLongitude, correctedObliquity);
  isSuccessful &= evaluate("Right ascension", rightAscension, expectedRightAscension);

  // calculate sun radius vector
  double sunRadiusVector = calculateSunRadiusVector(julianCenturyNumber);
  isSuccessful &= evaluate("Sun radius vector", sunRadiusVector, expectedSunRadiusVector);

  // calculate solar time with equation of time
  double eot = calculateEquationOfTime(julianCenturyNumber);
  double localSolarTime = calculateLocalSolarTime(timestamp_utc, eot, location_longitude);
  isSuccessful &= evaluate("Local solar time", localSolarTime, expectedLocalSolarTime);

  // calculate solar noon
  double localSolarNoon = calculateLocalSolarNoon(tz_offset, eot, location_longitude);
  isSuccessful &= evaluate("Local solar noon", localSolarNoon, expectedLocalSolarNoon);

  // calculate sunset time
  double sunsetTime = calculateSunriseSunsetTime(localSolarNoon, calculateSunsetHourAngle(declination, location_latitude));
  isSuccessful &= evaluate("Sunset time", sunsetTime, expectedSunsetTime);

  // calculate azimuth with hour angle and elevation
  double hourAngle = calculateHourAngle(localSolarTime);
  double elevation = calculateSolarElevation(hourAngle, declination, location_latitude);  // Sonnenhöhe in Radiant
  double azimuth = calculateSolarAzimuth(hourAngle, declination, elevation, location_latitude);
  isSuccessful &= evaluate("Azimut", azimuth, expectedAzimuth);

  // calculate corrected elevation
  double corrSolarElev = calculateCorrectedSolarElevation(elevation);
  isSuccessful &= evaluate("Corrected solar elevation", corrSolarElev, expectedCorrectedSolarElevation);

  // calculate daytime length
  double h0 = calculateSunriseHourAngle(declination, location_latitude);
  double lengthOfDay = calculateDaylightMinutes(h0);
  isSuccessful &= evaluate("Sunlight duration", lengthOfDay, expectedLengthOfDay);

  return isSuccessful;
}

boolean evaluate(String name, double value, double expectedValue) {
  boolean isCorrect = isEqual(value, expectedValue);
  if (!isCorrect) {
    Serial.println("\t" + name + ": " + String(value, 10) + " expected: " + String(expectedValue, 10));
  }
  return isCorrect;
}

boolean isEqual(double a, double b) {
  static const double ACCURACY = 10000.0;
  return abs((a - b) * ACCURACY) < 1;
}
