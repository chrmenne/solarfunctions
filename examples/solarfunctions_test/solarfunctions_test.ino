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
  test(0, -78.4678d, 1710896400, -5, 0.24215606d, 0.048230654d, 0.111248903d, 0.995917448d, 0.767061184d, 89.71348488d, 38.7638959d / 60.0, 0.514747295d, -80.30792179d, 726.6640024d, "Quito, spring equinox 2024-03-20 01:00 local time (UTC-5)");
  test(0, -78.4678d, 1710918000, -5, 0.24216290d, 0.14702914d, 0.33914008d, 0.9959874d, 0.767009661d, 89.85083414d, 398.8380991d / 60.0, 0.514695764d, 9.800096027d, 726.6640219d, "Quito, spring equinox 2024-03-20 07:00 local time (UTC-5)");
  test(0, -78.4678d, 1710939600, -5, 0.24216975d, 0.245811417d, 0.567001248d, 0.996057422d, 0.766958066d, 271.4544356d, 758.9124152d / 60.0, 0.514644156d, 80.27158859d, 726.6640613d, "Quito, spring equinox 2024-03-20 13:00 local time (UTC-5)");
  // Quito, fall equinox 2024-09-23, 01:00/07:00/13:00 local time (UTC-5)
  test(0, -78.4678d, 1727053200, -5, 0.24727584d, -0.28188216d, -179.3497922d, 1.003390741d, 0.756601971d, 91.21116461d, 53.8252014d / 60.0, 0.504288055d, -76.53942111d, 726.6640807d, "Quito, fall equinox 2024-09-23 01:00 local time (UTC-5)");
  test(0, -78.4678d, 1727074800, -5, 0.24728268d, -0.379220386d, -179.1252446d, 1.003320629d, 0.756541166d, 90.38996008d, 413.9127945d / 60.0, 0.504227226d, 13.5438534d, 726.664146d, "Quito, fall equinox 2024-09-23 07:00 local time (UTC-5)");
  test(0, -78.4678d, 1727096400, -5, 0.24728953d, -0.476566397d, -178.9006606d, 1.003250452d, 0.756480461d, 267.9593782d, 774.0002513d / 60.0, 0.504166492d, 76.49556112d, 726.6642305d, "Quito, fall equinox 2024-09-23 13:00 local time (UTC-5)");
  // Nuremberg, 2024-07-15, 04:06/14:00/22:54 local time (UTC+2/DST)
  test(49.734839d, 11.156555d, 1721016360, 2, 0.24535489d, 21.46187447d, 114.9304399d, 1.016450315d, 0.887717195d, 38.45394729d, 164.6076673d / 60.0, 0.556522453d, -9.971896105d, 953.8408549d, "Nuremberg 2024-07-15 04:06 local time (UTC+2/DST)");
  test(49.734839d, 11.156555d, 1721052000, 2, 0.24536619d, 21.39538562d, 115.3470636d, 1.016429856d, 0.887454915d, 198.5525646d, 758.567444d / 60.0, 0.556550386d, 60.66000295d, 953.0050422d, "Nuremberg 2024-07-15 14:00 local time (UTC+2/DST)");
  test(49.734839d, 11.156555d, 1721084040, 2, 0.24537634d, 21.33473278d, 115.721293d, 1.01641078d, 0.887214914d, 325.1903798d, 1292.532528d / 60.0, 0.556574634d, -11.74516359d, 952.244009d, "Nuremberg 2024-07-15 22:54 local time (UTC+2/DST)");
  // Nuremberg, 2024-01-15, 08:24/13:00/16:36 local time (UTC+1)
  test(49.734839d, 11.156555d, 1705307040, 1, 0.24037805d, -21.19812267d, -63.45519523d, 0.983614571d, 0.695631064d, 126.0263058d, 479.4656921d / 60.0, 0.517037714d, 1.713437273d, 514.34885d, "Nuremberg 2024-01-15 08:24 local time (UTC+1)");
  test(49.734839d, 11.156555d, 1705323600, 1, 0.24038330d, -21.16329308d, -63.2491759d, 0.983625434d, 0.695823468d, 188.7095982d, 755.3970968d / 60.0, 0.517085349d, 18.71467248d, 514.7657826d, "Nuremberg 2024-01-15 13:00 local time (UTC+1)");
  test(49.734839d, 11.156555d, 1705336560, 1,  0.24038741d, -21.13586177d, -63.08801441d, 0.983634064d, 0.69597448d, 236.0834078d, 971.3436944d / 60.0, 0.517122434d, 0.48578732d, 515.0938899d, "Nuremberg 2024-01-15 16:36 local time (UTC+1)");
  // Arctic Circle, summer solstice 2024-06-21, 00:00/12:00/18:00 local time (UTC+0)
  test(66.5622d, 0, 1718928000, 0, 0.24469541d, 23.43855495d, 90.13761524d, 1.016241671d, INVALID_VALUE, 359.5834118d, 1438.183783d / 60.0, 0.501261261d, 0.483152831d, 1440, "Arctic Circle, summer solstice 2024-06-21, 00:00 local time (UTC+0)");
  test(66.5622d, 0, 1718971200, 0, 0.24470910d, 23.43723796d, 90.65763876d, 1.01627395d, INVALID_VALUE, 179.3541348d, 718.0751405d / 60.0, 0.501336708d, 46.88905947d, 1440, "Arctic Circle, summer solstice 2024-06-21, 12:00 local time (UTC+0)");
  test(66.5622d, 0, 1718992800, 0, 0.24471595d, 23.43593364d, 90.9176327d, 1.016289653d, INVALID_VALUE, 279.3416442d, 1078.020889d / 60.0, 0.501374383d, 21.63645706d, 1440, "Arctic Circle, summer solstice 2024-06-21, 18:00 local time (UTC+0)");
  // Arctic Circle, winter solstice 2024-12-21, 00:00/12:00/18:00 local time (UTC+0)
  test(66.5622d, 0, 1734739200, 0, 0.24970568d, -23.43802069d, -90.42731352d, 0.983766103d, 0.543729144d, 0.646212104d, 1.925876741d / 60.0, 0.498662586d, -46.8693365d, 129.7979624d, "Arctic Circle, winter solstice 2024-12-21, 00:00 local time (UTC+0)");
  test(66.5622d, 0, 1734782400, 0, 0.24971937d, -23.43855019d, -89.87235135d, 0.983732575d, 0.543889106d, 180.3848909d, 721.6780243d / 60.0, 0.498834705d, 0.480822517d, 129.7566742d, "Arctic Circle, winter solstice 2024-12-21, 12:00 local time (UTC+0)");
  test(66.5622d, 0, 1734804000, 0, 0.24972621d, -23.43807926d, -89.59485665d, 0.983716277d, 0.543987951d, 260.562598d, 1081.554049d / 60.0, 0.498920799d, -21.54162994d, 129.7933957d, "Arctic Circle, winter solstice 2024-12-21, 18:00 local time (UTC+0)");
  // North Pole, summer solstice 2024-06-21, 12:00 local time (UTC+0) (polar day)
  test(90, 0, 1718971200, 0, 0.24470910d, 23.43723796d, 90.65763876d, 1.01627395d, INVALID_VALUE, 180, 718.0751405d / 60.0, 0.501336708d, 23.47422922d, 1440, "North Pole, summer solstice 2024-06-21, 12:00 local time (UTC+0) (polar day)");
  // North Pole, winter solstice 2024-12-21, 12:00 local time (UTC+0) (polar night)
  test(90, 0, 1734782400, 0, 0.24971937d, -23.43855019d, -89.87235135d, 0.983732575d, INVALID_VALUE, INVALID_VALUE, 721.6780243d / 60.0, 0.498834705d, -23.42529106d, 0, "North Pole, winter solstice 2024-12-21, 12:00 local time (UTC+0) (polar night)");
  // South Pole, summer solstice 2024-06-21, 12:00 local time (UTC+0) (polar night)
  test(-90, 0, 1718971200, 0, 0.24470910d, 23.43723796d, 90.65763876d, 1.01627395d, INVALID_VALUE, INVALID_VALUE, 718.0751405d / 60.0, 0.501336708d, -23.42392801d, 0, "South Pole, summer solstice 2024-06-21, 12:00 local time (UTC+0) (polar night)");
  // South Pole, winter solstice 2024-12-21, 12:00 local time (UTC+0) (polar day)
  test(-90, 0, 1734782400, 0, 0.24971937d, -23.43855019d, -89.87235135d, 0.983732575d, INVALID_VALUE, 0, 721.6780243d / 60.0, 0.498834705d, 23.47553916d, 1440, "South Pole, winter solstice 2024-12-21, 12:00 local time (UTC+0) (polar day)");

  // loop eternally
  Serial.println("");
  Serial.println("");
  delay(5000);
}

// Test function. To calculate the values expected, every function in the library needs to be used at least once. 
// Detailed output is only printed on mismatches.
boolean test(double location_latitude,
             double location_longitude,
             long timestamp, 
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

  long timestamp_utc = timestamp - (tz_offset * 3600);

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

