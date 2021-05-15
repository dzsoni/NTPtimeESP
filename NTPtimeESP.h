/*

   NTPtime for ESP8266/ESP32
   This routine gets the unixtime from a NTP server and adjusts it to the time zone and the
   Middle European summer time if requested

  Author: Andreas Spiess V1.0 2016-6-28

  Based on work from John Lassen: http://www.john-lassen.de/index.php/projects/esp-8266-arduino-ide-webconfig

*/
#ifndef NTPtime_h
#define NTPtime_h

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif
#include <WiFiUdp.h>
#include <SimpleJsonParser.h>

struct strDateTime
{
  byte hour;
  byte minute;
  byte second;
  int year;
  byte month;
  byte day;
  byte dayofWeek;
  unsigned long epochTime =0;
  boolean valid=false;
};
/** @class NTPtime
 * \brief NTPtime class
 * \details NTPtime class get time from a time server
 * 
 * @param[in]   str     Holds the name of the time server or the jsonfile name
 *                      depends on value of mode
 * @param[out]  mode    0-timeserver 1-json file name
 * 
 * Jsonfile example:
 * {"NTPserver":"europe.pool.ntp.org","UTCh":"1","UTCm":"0","extratsh":"ST"}
 * 
 * NTPserver    = your NTP server
 * UTCh         = Time zone -12 to +14 
 * UTCm         = minutes shift
 * extratsh     = None/ST/DST         None,Summmer Time,Daylight Save Time
*/
class NTPtime
{
public:
  SimpleJsonParser _sjsonp;
  NTPtime(String str = "", byte mode = 0);
  strDateTime getNTPtime(int8_t _timeZoneHour, uint8_t _timeZoneMin, int _DayLightSaving);
  strDateTime getNTPtime();
  strDateTime ConvertUnixTimestamp(unsigned long _tempTimeStamp);
  void printDateTime(strDateTime _dateTime);
  String  getNTPServer(){return _NTPserver;};
  bool setSendInterval(unsigned long sendInterval); // in seconds
  bool setRecvTimeout(unsigned long recvTimeout);   // in seconds

private:
  bool _sendPhase;
  unsigned long _sentTime;
  unsigned long _sendInterval;
  unsigned long _recvTimeout;

  String _NTPserver;
  int _utczone;
  int _utcmin;
  int _stdst; //0=neither, 1=Summer Time, 2=Daylight Save Time

  boolean summerTime(unsigned long _timeStamp);
  boolean daylightSavingTime(unsigned long _timeStamp);

  /**
   *adjustTimeZone(unsigned long _timeStamp, int8_t _timeZoneHour, uint8_t _timZoneMin, int _DayLightSavingSaving)
   *
   *@param[in]  _timeStamp      Unix timestamp 
   *@param[in]  _timeZoneHour   UTC timezone hour difference
   *@param[in]  _timeZoneMin    UTC timezone minutes shift
   *@param[in]  _DayLightSavingSaving 0- None, 1- Summer Time 2-DayLight Save Time
   *@param[out] Adjusted unix timestamp
   */
  unsigned long adjustTimeZone(unsigned long _timeStamp, int8_t _timeZoneHour, uint8_t _timZoneMin, int _DayLightSavingSaving);

  WiFiUDP UDPNTPClient;
};
#endif
