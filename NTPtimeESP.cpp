/*
NTP
This routine gets the unixtime from a NTP server and adjusts it to the time zone and the
Middle European summer time if requested

Author: Andreas Spiess V1.0 2016-5-28

Based on work from John Lassen: http://www.john-lassen.de/index.php/projects/esp-8266-arduino-ide-webconfig

*/

#include <Arduino.h>
#include "NTPtimeESP.h"
#include "struct_strDateTime.h"


#define SEC_TO_MS               1000
#define RECV_TIMEOUT_DEFAULT    1   // 1 second
#define SEND_INTRVL_DEFAULT     1   // 1 second
#define MAX_SEND_INTERVAL       60  // 60 seconds
#define MAC_RECV_TIMEOUT        60	// 60 seconds

const int NTP_PACKET_SIZE = 48;
byte _packetBuffer[NTP_PACKET_SIZE];


bool NTPtime::setSendInterval(unsigned long sendInterval)
{
	bool retVal = false;
	if (sendInterval <= MAX_SEND_INTERVAL)
	{
		_sendInterval = sendInterval * SEC_TO_MS;
		retVal = true;
	}
	return retVal;
}

bool NTPtime::setRecvTimeout(unsigned long recvTimeout)
{
	bool retVal = false;
	if (recvTimeout <= MAC_RECV_TIMEOUT)
	{
		_recvTimeout = recvTimeout * SEC_TO_MS;
		retVal = true;
	}
	return retVal;
}

NTPtime::NTPtime(String str, byte mode)
{
	_NTPserver 	= str;
	_sendPhase 	= true;
	_sentTime 	= 0;
	_sendInterval = SEND_INTRVL_DEFAULT * SEC_TO_MS;
	_recvTimeout  = RECV_TIMEOUT_DEFAULT * SEC_TO_MS;
	_utcmin 	= 0;
	_utchour 	= 0;
	_stdst 		= 0;

	if (mode == 1)
	{
		String json = _sjsonp.fileToString(str);
		_NTPserver  = _sjsonp.getJSONValueByKeyFromString(json, "NTPserver");
		_utchour    = _sjsonp.getJSONValueByKeyFromString(json, "UTCh").toInt();
		_utcmin     = (uint8_t)abs(_sjsonp.getJSONValueByKeyFromString(json, "UTCm").toInt());
		String tsh  = _sjsonp.getJSONValueByKeyFromString(json, "extratsh");
		if (tsh == "ST")
		{
			_stdst = 1;			//Summer Time
		}
		else if (tsh == "DST")
		{
			_stdst = 2;			//Daylight Saving Time
		}
		else
		{
			_stdst = 0;
		}
	}
}

void NTPtime::printDateTime(strDateTime _dateTime)
{
	if (_dateTime.valid)
	{
		Serial.print(_dateTime.year);
		Serial.print("-");
		Serial.print(_dateTime.month);
		Serial.print("-");
		Serial.print(_dateTime.day);
		Serial.print(" ");
		Serial.print(_dateTime.hour);
		Serial.print(":");
		Serial.print(_dateTime.minute);
		Serial.print(":");
		Serial.print(_dateTime.second);
		Serial.println();
	}
	else
	{
#ifdef DEBUG_ON
		Serial.println("Invalid time !!!");
		Serial.println("");
#endif
	}
}

//
// Summertime calculates the daylight saving time for middle Europe. Input: Unixtime in UTC
//
boolean NTPtime::summerTime(unsigned long _timeStamp)
{

	strDateTime _tempDateTime;
	_tempDateTime.setFromUnixTimestamp(_timeStamp);
	// printTime("Innerhalb ", _tempDateTime);

	if (_tempDateTime.month < 3 || _tempDateTime.month > 10)
		return false; // keine Sommerzeit in Jan, Feb, Nov, Dez
	if (_tempDateTime.month > 3 && _tempDateTime.month < 10)
		return true; // Sommerzeit in Apr, Mai, Jun, Jul, Aug, Sep
	if (((_tempDateTime.month == 3) && ((_tempDateTime.hour + 24 * _tempDateTime.day) >= (3 + 24 * (31 - (5 * _tempDateTime.year / 4 + 4) % 7)))) || ((_tempDateTime.month == 10) && (_tempDateTime.hour + 24 * _tempDateTime.day) < (3 + 24 * (31 - (5 * _tempDateTime.year / 4 + 1) % 7))))
		return true;
	else
		return false;
}

boolean NTPtime::daylightSavingTime(unsigned long _timeStamp)
{

	strDateTime _tempDateTime;
	_tempDateTime.setFromUnixTimestamp(_timeStamp);

	// here the US code
	//return false;
	// see http://stackoverflow.com/questions/5590429/calculating-daylight-saving-time-from-only-date
	// since 2007 DST begins on second Sunday of March and ends on first Sunday of November.
	// Time change occurs at 2AM locally
	if (_tempDateTime.month < 3 || _tempDateTime.month > 11)
		return false; //January, february, and december are out.
	if (_tempDateTime.month > 3 && _tempDateTime.month < 11)
		return true;														//April to October are in
	int previousSunday = _tempDateTime.day - (_tempDateTime.dayofWeek - 1); // dow Sunday input was 1,
	// need it to be Sunday = 0. If 1st of month = Sunday, previousSunday=1-0=1
	//int previousSunday = day - (dow-1);
	// -------------------- March ---------------------------------------
	//In march, we are DST if our previous Sunday was = to or after the 8th.
	if (_tempDateTime.month == 3)
	{ // in march, if previous Sunday is after the 8th, is DST
		// unless Sunday and hour < 2am
		if (previousSunday >= 8)
		{ // Sunday = 1
			// return true if day > 14 or (dow == 1 and hour >= 2)
			return ((_tempDateTime.day > 14) ||
					((_tempDateTime.dayofWeek == 1 && _tempDateTime.hour >= 2) || _tempDateTime.dayofWeek > 1));
		} // end if ( previousSunday >= 8 && _dateTime.dayofWeek > 0 )
		else
		{
			// previousSunday has to be < 8 to get here
			//return (previousSunday < 8 && (_tempDateTime.dayofWeek - 1) = 0 && _tempDateTime.hour >= 2)
			return false;
		} // end else
	}	  // end if (_tempDateTime.month == 3 )

	// ------------------------------- November -------------------------------

	// gets here only if month = November
	//In november we must be before the first Sunday to be dst.
	//That means the previous Sunday must be before the 2nd.
	if (previousSunday < 1)
	{
		// is not true for Sunday after 2am or any day after 1st Sunday any time
		return ((_tempDateTime.dayofWeek == 1 && _tempDateTime.hour < 2) || (_tempDateTime.dayofWeek > 1));
		//return true;
	} // end if (previousSunday < 1)
	else
	{
		// return false unless after first wk and dow = Sunday and hour < 2
		return (_tempDateTime.day < 8 && _tempDateTime.dayofWeek == 1 && _tempDateTime.hour < 2);
	} // end else
} // end boolean NTPtime::daylightSavingTime(unsigned long _timeStamp)

  /**
   *adjustTimeZone(unsigned long timeStamp, int8_t timeZoneHour, uint8_t timZoneMin, int DayLightSavingSaving)
   *
   *@param[in]  timeStamp      Unix timestamp 
   *@param[in]  timeZoneHour   UTC timezone hour difference
   *@param[in]  timeZoneMin    UTC timezone minutes shift
   *@param[in]  DayLightSavingSaving 0- None, 1- Summer Time 2-DayLight Save Time
   *@param[out] Adjusted unix timestamp
   */
unsigned long NTPtime::adjustTimeZone(unsigned long timeStamp, int8_t timeZoneHour, uint8_t timeZoneMin, int DayLightSaving)
{
	timeStamp+=(unsigned long)(timeZoneHour * 3600);
	if(timeZoneHour<0)
	{
	timeStamp -= (unsigned long)(timeZoneMin *60);
	}
	else
	{
    timeStamp+=(unsigned long)(timeZoneMin *60);
	}
	if (DayLightSaving == 1 && summerTime(timeStamp))
		timeStamp += 3600; // European Summer time
	if (DayLightSaving == 2 && daylightSavingTime(timeStamp))
		timeStamp += 3600; // US daylight time
	return timeStamp;
}

/* The time of a time zone is an offset from UTC. Most adjacent time zones are exactly one hour apart
   ,but there are not round hour differnce zones too. Use timeZoneMin to set not round hour.
   If _DayLightSaving is true, time will be adjusted accordingly.
   Use returned time only after checking "ret.valid" flag
*/
strDateTime NTPtime::getNTPtime(int8_t timeZoneHour, uint8_t timeZoneMin, int DayLightSaving)
{
	_utchour = timeZoneHour;
	_utcmin  = timeZoneMin;
	_stdst   = DayLightSaving;

	int cb;
	strDateTime dateTime;
	unsigned long unixTime = 0;
	unsigned long currentTimeStamp;

	if (_sendPhase)
	{
		if (_sentTime && ((unsigned long)(millis() - _sentTime) < _sendInterval))
		{
			return dateTime;
		}

		_sendPhase = false;
		UDPNTPClient.begin(1337); // Port for NTP receive

#ifdef DEBUG_ON
		IPAddress timeServerIP;
		WiFi.hostByName(_NTPserver.c_str(), timeServerIP);
		Serial.println();
		Serial.println(timeServerIP);
		Serial.println("Sending NTP packet");
#endif

		memset(_packetBuffer, 0, NTP_PACKET_SIZE);
		_packetBuffer[0] = 0b11100011; // LI, Version, Mode
		_packetBuffer[1] = 0;		   // Stratum, or type of clock
		_packetBuffer[2] = 6;		   // Polling Interval
		_packetBuffer[3] = 0xEC;	   // Peer Clock Precision
		_packetBuffer[12] = 49;
		_packetBuffer[13] = 0x4E;
		_packetBuffer[14] = 49;
		_packetBuffer[15] = 52;
		UDPNTPClient.beginPacket(_NTPserver.c_str(), 123);
		UDPNTPClient.write(_packetBuffer, NTP_PACKET_SIZE);
		UDPNTPClient.endPacket();

		_sentTime = millis();
	}
	else
	{
		cb = UDPNTPClient.parsePacket();
		if (cb == 0)
		{
			if ((unsigned long)(millis() - _sentTime) > _recvTimeout)
			{
				_sendPhase = true;
				_sentTime = 0;
			}
		}
		else
		{
#ifdef DEBUG_ON
			Serial.print("NTP packet received, length=");
			Serial.println(cb);
#endif

			UDPNTPClient.read(_packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
			unsigned long highWord = word(_packetBuffer[40], _packetBuffer[41]);
			unsigned long lowWord = word(_packetBuffer[42], _packetBuffer[43]);
			unsigned long secsSince1900 = highWord << 16 | lowWord;
			const unsigned long seventyYears = 2208988800UL;
			unixTime = secsSince1900 - seventyYears;
			if (secsSince1900 > 0)
			{
				currentTimeStamp = adjustTimeZone(unixTime, timeZoneHour,  timeZoneMin, DayLightSaving);
				dateTime.setFromUnixTimestamp(currentTimeStamp);
				dateTime.valid = true;
			}
			else
				dateTime.valid = false;

			_sendPhase = true;
		}
	}

	return dateTime;
}
 strDateTime NTPtime::getNTPtime()
 {
  return getNTPtime(_utchour,_utcmin,_stdst);
 }