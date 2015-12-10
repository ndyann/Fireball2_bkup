#include "sun.hpp"

double  sun_elevation(struct tm t,float lat, float lon) {
    int year    = t.tm_year,
        month   = t.tm_mon,
        day     = t.tm_mday,
        hour    = t.tm_hour,
        min     = t.tm_min,
        sec     = t.tm_sec,
        leapDays;
    float delta, leap, jd, time, mnLong, mnAnom, num, den, ra, gmst, lmst, ha, el, ecLong, oblqEc, dec;
    
    float twopi = 2 * pi;
    float deg2rad = pi / 180;
    
    //Get day of year, including leap year if applicable
    const int monthDays[] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334}; //First 0 is dummy var, no month 0
    day = day + monthDays[month];
    leapDays = (year%4 == 0) && ((year%400 == 0) || (year%100 != 0));
    if (leapDays && (month > 2)) ++day;
    
    //Get Julian date
    hour = hour + 7 + min / 60 + sec / 3600; //Converts hour from MST to GST
    delta = year - 1949;
    leap = trunc(delta / 4);
    jd = 32916.5 + (delta * 365) + leap + day + (hour / 24);
    
    //The input to the Astronomer's almanach is the difference between the Julian date and JK 2451545.0 (noon, 1 Jan 2000)
    time = jd - 51545;
    
    //Ecliptic coordinates
    
    //Mean longitude
    mnLong = 280.460 + .9856474 * time;
    mnLong = fmodf(mnLong,360.);
    if(mnLong < 0) { mnLong += 360; }
    
    //Mean anomaly
    mnAnom = 357.528 + .9856003 * time;
    mnAnom = fmodf(mnAnom,360.);
    if(mnAnom < 0) { mnAnom += 360; }
    mnAnom = mnAnom * deg2rad;
    
    //Ecliptic longitude and obliquity of ecliptic
    ecLong = mnLong + 1.915 * sin(mnAnom) + 0.020 * sin(2 * mnAnom);
    ecLong = fmodf(ecLong,360.);
    if(ecLong < 0) { ecLong += 360; }
    oblqEc = 23.439 - 0.0000004 * time;
    ecLong = ecLong * deg2rad;
    oblqEc = oblqEc * deg2rad;
    
    //Celestial coordinates
    
    //Right ascension and declination
    num = cos(oblqEc) * sin(ecLong);
    den = cos(ecLong);
    float bob = num / den;
    ra = atan(bob);
    if (den < 0) { ra += pi; }
    if (den >= 0 && num < 0) { ra += twopi; }
    dec = asin(sin(oblqEc) * sin(ecLong));
    
    //Local coordinates
    
    //Greenwich mean sidereal time
    gmst = 6.697375 + .0657098242 * time + hour;
    gmst = fmodf(gmst,24.);
    if (gmst < 0) { gmst += 24; }
    
    //Local mean sidereal time
    lmst = gmst + lon / 15;
    lmst = fmodf(lmst,24.);
    if (lmst < 0) { lmst += 24; }
    lmst = lmst * 15 * deg2rad;
    
    //Hour angle
    ha = lmst - ra;
    if (ha < -pi) { ha += twopi; }
    if (ha > pi) { ha -= twopi; }
    
    //Latitude to radians
    lat = lat * deg2rad;
    
    //Elevation
    el = asin(sin(dec) * sin(lat) + cos(dec) * cos(lat) * cos(ha));
    
    el = el / deg2rad;
    lat = lat / deg2rad;
    
    return el;
}
bool    safe_sun(struct tm t,float lat, float lon)      {
    return (sun_elevation(t,lat,lon) < 49);
}