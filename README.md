# solarfunctions
An Arduino library using formulae by Jean Meeus and the National Oceanic and Atmospheric Administration (NOAA) to precisely determine various solar metrics. The calculations are based on geographic latitude and longitude of the observer (positive numbers for Northern latitude and Eastern longitude, negative numbers for Southern latitude and Western longitude) and UTC time.

I wrote this library because I could only find libraries that either didn't compute what I needed or stated to be imprecise. It was tested with values provided by the [Solar Calculator](https://gml.noaa.gov/grad/solcalc/) by NOAA. It is as precise as the specifications for double values on the platform used will allow (32 or 64 bit). The difference in precision between 32 and 64 bit should be irrelevant for most practical purposes and usually amounts to around just a fraction of second of a degree.

The library has no dependencies (besides Arduino.h) and should be easy to use, although it can be a bit difficult for a novice (like me) to understand what is necessary to calculate what. In most cases though, the input required is simply the time (as Julian Century Number) and the location.

All angular values are taken and returned in (normalized) degrees since that's what they are commonly stated in. "Normalized" means that angles like 705° are either delivered as 345° (normalized to 0°-360°) or -15° (normalized to +/- 180°), depending on the common usage of this value. The library inclues functions to convert and normalize degrees and radians values.

I refrained from adding convenience functions (so far). Everything could always be calculated by inserting timestamp and location and following some steps, so there is no technical reason I couldn't add a `calculateCorrectedSolarElevation(long timestamp, long latitude)` function which calculated the Julian Century Number, the declination, the hour angle and the elevation". But there would be many of them, they would clutter the library and also lead to repetitive calculation steps. On the other hand, for every parameter a function needs except location, time and timezone offset, there's an easy-to-find `calculate...`-function that provides this parameter. So it should be simple enough to derive the necessary cascade of function calls, as the examples will show.

## Examples
Let's assume a location in the city of **Nuremberg, Germany, latitude 49.734839° N, longitude 11.156555° E**. The date is **July 15th 2024, 14:00 o'clock Central European Summer Time (CEST, daylight savings in effect)**, which resembles 12:00 UTC. The Unix timestamp for this datetime is **1721044800 (UTC)**, as the timezone offset is -1 hour (3600 seconds) for CET and another -1 hour (3600 seconds) for DST. Calculation: `1721052000 (local timestamp) - 3600 (timezone offset) - 3600 (DST) = 1721044800`

```c
// data we do know
double location_latitude  = 49.734839;
double location_longitude = 11.156555;
long time_utc             = 1721044800;
double timezone_offset    = -2; // the local time zone's offset from UTC. -1 hour for CET, -1 hour for DST.
```

First, we calculate the Julian Century Number, which is the time unit used in all the formulae. It resembles the fraction of a century that has passed since noon on 2000-01-01 (UTC).

```c
double julianCenturyNumber = calculateJulianCenturyNumber(time_utc); // = 0.24536619
```

### The Sun's Position (Corrected Elevation And Azimuth)</h3>
Let's say we have a telescope or moveable solar panel and want to know where to point it at this moment, so we need to calculate the altitude ("elevation") and direction ("azimuth") the Sun. Calculating the elevation for a specific time and location requires
- the hour angle for this time (the angular difference between the Sun's current position and the highest point at noon, on a circle around the ecliptic),</li>
- the declination (the angle between Earth's equator and the line between Earth and Sun), and</li>
- the latitude.

Calculating the azimuth requires the same plus the elevation. So let's start with the hour angle, which in turn requires the local solar time (the time a sundial would show). The solar time in turn requires the Equation of Time, which is the difference in minutes between the mean  (theoretical) time and the apparent solar time. Basically, it's the time span between e.&nbsp;g. a clock saying &quot;it's noon&quot; and a sundial saying &quot;it's noon&quot;.</p>

```c
double equationOfTime = calculateEquationOfTime(julianCenturyNumber); // let's start with the Equation of Time: = -6.05877415 minutes
double localSolarTime = calculateLocalSolarTime(time_utc, equationOfTime, location_longitude); // = 12.64279076 hours
double hourAngle = calculateHourAngle(localSolarTime); // = 9.64186146 degrees
// now let's move on to calculate the elevation from hour angle, declination and latitude: 
double declination = calculateDeclination(julianCenturyNumber); // = 21.39538652 degrees
double elevation = calculateSolarElevation(hourAngle, declination, location_latitude); // = 60.65093229 degrees
// since we want to point something at the Sun where it appears to be, not where it physically is, so we need to get the elevation corrected for atmospheric refraction
double correctedSolarElevation = calculateCorrectedSolarElevation(elevation); // =  60.66000374 degrees
// and now the azimuth
double azimuth = calculateSolarAzimuth(hourAngle, declination, elevation, location_latitude); // 89.71348454 degrees
```
That's it.

### Daylight Information (Sunrise, Sunset, Daylight)
Now let's assume we want to plan outdoor activities. We would like to know the the duration of daylight on that day as well as the time of sunrise and sunset.

The daylight duration needs the hour angle of sunrise/sunset. Since the horizon is level, they're the same in value, just in different directions from the vertical "noon" line - negative angle for sunrise, positive angle for sunset. So it doesn't matter which one is used. The sunrise/sunset hour angle needs the declination to be calculated, we already have this value from calculating the elevation.

The sunrise and sunset times need the sunrise and sunset hour angles as well as the solar noon. The solar noon in turn needs the Equation of Time. We already have that, too.

```c
// amount of daylight
double sunriseHourAngle = calculateSunriseHourAngle(declination, location_latitude); // = -119.03050254 degrees
double lengthOfDay = calculateDaylightMinutes(sunriseHourAngle); // = 726.66423054 minutes
// sunrise/sunset
double sunsetHourAngle = calculateSunsetHourAngle(declination, location_latitude);  // = 119.03050254 degrees
double localSolarNoon = calculateLocalSolarNoon(timezone_offset, equationOfTime, location_longitude); // 0.55655038 of a day --> 0.55655 * 24 = 13.3572 hours (13:21 local time)
double sunriseTime = calculateSunriseSunsetTime(localSolarNoon, sunriseHourAngle); // = 0.22593435 of a day --> 0.22593 * 24 = 5.42232 hours (05:25:20 local time)
double sunsetTime = calculateSunriseSunsetTime(localSolarNoon, sunsetHourAngle); // = 0.88745492 of a day --> 0.22593 * 24 = 21.2988 hours (21:17:56 local time)
```
