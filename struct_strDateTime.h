
#ifndef struct_strDateTime_h
#define struct_strDateTime_h

#include <Arduino.h>

#ifndef LEAPYEAR_
#define LEAPYEAR_
#define LEAP_YEAR(Y) (((1970 + Y) > 0) && !((1970 + Y) % 4) && (((1970 + Y) % 100) || !((1970 + Y) % 400)))
#endif

static const uint8_t monthDays[] PROGMEM = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
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
    {
        uint32_t time,years;
        uint8_t  months, monthLength;
        unsigned long days;

        epochTime = tempTimeStamp;

        time      = (uint32_t)tempTimeStamp;
        second    = time % 60;
        time     /= 60; // now it is minutes
        minute    = time % 60;
        time     /= 60; // now it is hours
        hour      = time % 24;
        time     /= 24;                       // now it is _days
        dayofWeek = ((time + 4) % 7) + 1; // Sunday is day 1

        years = 0;
        days = 0;
        while ((unsigned)(days += (LEAP_YEAR(years) ? 366 : 365)) <= time)
        {
            years++;
        }

        year=years+1970;
        days -= LEAP_YEAR(years) ? 366 : 365;
        time -= days; // now it is days in this year, starting at 0

        days = 0;
        months = 0;
        monthLength = 0;
        for (months = 0; months < 12; months++)
        {
            if (months == 1)
            { // february
                if (LEAP_YEAR(years))
                {
                    monthLength = 29;
                }
                else
                {
                    monthLength = 28;
                }
            }
            else
            {
                monthLength = pgm_read_byte(monthDays+months);

            }
            
            if (time >= monthLength)
            {
                time -= monthLength;
            }
            else
            {
                break;
            }
        }
        month = months + 1; // jan is month 1
        day   = time + 1;   // day of month
        return;
    }
    
    void printDateTime()
    {
        if(valid)
        {
        Serial.printf("%04u/%02u/%02u %02u:%02u:%02u",year,month,day,hour,minute,second);
        }
    }
};

#endif