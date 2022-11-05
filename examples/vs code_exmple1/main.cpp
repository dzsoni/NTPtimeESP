#include <Arduino.h>
#include <NTPtimeESP.h>



NTPtime NTPch("europe.pool.ntp.org",0); // Choose server pool as required ,  mode  0-timeserver 1-json file name
char *ssid      = "";               // Set you WiFi SSID
char *password  = "";               // Set you WiFi password

strDateTime dateTime;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booted");
  Serial.println("Connecting to Wi-Fi");

  WiFi.mode(WIFI_STA);
  WiFi.begin (ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("WiFi connected.");
}

void loop() {
  //Examples:
  //Set for India timezone(UTC+5:30)
  //_DayLightSaving: 0=none, 1=Summer Time (EU), 2=Daylight Save Time (USA)
  //dateTime = NTPch.getNTPtime(5,30,0);
  
  //Set for NewYork UTC-5:00) with DST
  //_DayLightSaving: 0=none, 1=Summer Time (EU), 2=Daylight Save Time (USA)
  //dateTime = NTPch.getNTPtime(-5,0,2);

  //Set for Germany  UTC+1:00) with Summer time
  //_DayLightSaving: 0=none, 1=Summer Time (EU), 2=Daylight Save Time (USA)
  dateTime = NTPch.getNTPtime(1,0,1);


  // check dateTime.valid before using the returned time
  if(dateTime.valid)
  {
    dateTime.printDateTime();
    Serial.println("");
    byte actualHour = dateTime.hour;
    byte actualMinute = dateTime.minute;
    byte actualsecond = dateTime.second;
    int actualyear = dateTime.year;
    byte actualMonth = dateTime.month;
    byte actualday =dateTime.day;
    byte actualdayofWeek = dateTime.dayofWeek;
  }
}