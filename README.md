# NTPtimeESP


This library queries the NTP time service and returns the current time in a structure. The time can be automatically set according to time zone, European Summer Time and American Daylight Saving Time.

```
struct strDateTime
{
    byte hour;
    byte minute;
    byte second;
    int year;
    byte month;
    byte day;
    byte dayofWeek;

    unsigned long epochTime = 0;
    boolean valid = false;

    void setFromUnixTimestamp(unsigned long tempTimeStamp)
    {...};
    void printDateTime()
    {...};
};
```
# Changes

Compared to the original (SensorsIot/NTPtimeESP), I made some changes:

- I moved the printDateTime() function to the strDateTime structure. ("selfprint")

- You can also load the strDateTime structure from unxitime format. setFromUnixTimestamp(unsigned long tempTimeStamp)

-You can also store the time in unixtime format in it. (This has been useful for me in some RTC libraries.)

-The time of a time zone is an offset from UTC. Most adjacent time zones are exactly one hour apart, but there are not round hour differnce zones too.
You can adjust it.(getNTPtime(int8_t timeZoneHour, uint8_t timeZoneMin, int DayLightSaving))

-Summer Time changeover is binded to the UTC time. (works in Western European Time  (WET), Central European Time (CET) and Eastern European Time (EET) too.) 
 
