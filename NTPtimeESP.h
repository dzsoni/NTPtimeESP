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
#include <SimpleJsonParser.h> //https://github.com/dzsoni/SimpleJsonParser
#include "struct_strDateTime.h"

#ifndef LEAPYEAR_
#define LEAPYEAR_
#define LEAP_YEAR(Y) (((1970 + Y) > 0) && !((1970 + Y) % 4) && (((1970 + Y) % 100) || !((1970 + Y) % 400)))
#endif

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
 * NTPserver    = NTP server
 * UTCh         = Time zone -12 to +14 
 * UTCm         = minutes shift
 * extratsh     = None/ST/DST         None,Summmer Time,Daylight Save Time
*/
class NTPtime
{
  private:

  bool           _sendPhase;
  unsigned long  _sentTime;
  unsigned long  _sendInterval;
  unsigned long  _recvTimeout;

  String  _NTPserver;
  int8_t  _utchour;
  uint8_t _utcmin;
  int8_t  _stdst; //0=neither, 1=Summer Time, 2=Daylight Save Time

public:

  SimpleJsonParser  _sjsonp;
  strDateTime       getNTPtime(int8_t timeZoneHour, uint8_t timeZoneMin, int DayLightSaving);
  strDateTime       getNTPtime();
  unsigned long     adjustTimeZone(unsigned long timeStamp, int8_t timeZoneHour, uint8_t timZoneMin, int DayLightSavingSaving);
  

  String            getNTPServer(){return _NTPserver;};
  int8_t            getUtcHour() {return _utchour;};
  uint8_t           getUtcMin()  {return _utcmin;};
  int8_t            getSTDST()   {return _stdst;};

  void              setNTPServer(String server) {_NTPserver = server;};
  void              setUtcHour(int8_t hour) {_utchour = hour;};
  void              setUtcMin(uint8_t min) {_utcmin = min;};
  void              setSTDST(int8_t stdst) {_stdst = stdst;};

  bool              setSendInterval(unsigned long sendInterval); // in seconds
  bool              setRecvTimeout(unsigned long recvTimeout);   // in seconds

  NTPtime(String str = "", byte mode = 0);
  
private:
  boolean           summerTime(unsigned long _timeStamp);
  boolean           daylightSavingTime(unsigned long _timeStamp);
  WiFiUDP           UDPNTPClient;
};
#endif
