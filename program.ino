/*
  deltahat
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

/*  credentials.h tiene que estar en su carpeta de biblioteca. su contenido es:

#define mySSID "SuSSID"
#define myPASSWORD "SuContraseña"

Si no desea este archivo, codifique sus credenciales en este boceto

*/

#ifndef HELPERS_H
#define HELPERS_H
// #define ESP_12


//
// Check the Values is between 0-255
//
boolean checkRange(String Value)
{
  if (Value.toInt() < 0 || Value.toInt() > 255)
  {
    return false;
  }
  else
  {
    return true;
  }
}



void WriteStringToEEPROM(int beginaddress, String string)
{
  char  charBuf[string.length() + 1];
  string.toCharArray(charBuf, string.length() + 1);
  for (int t =  0; t < sizeof(charBuf); t++)
  {
    EEPROM.write(beginaddress + t, charBuf[t]);
  }
}
String  ReadStringFromEEPROM(int beginaddress)
{
  volatile byte counter = 0;
  char rChar;
  String retString = "";
  while (1)
  {
    rChar = EEPROM.read(beginaddress + counter);
    if (rChar == 0) break;
    if (counter > 31) break;
    counter++;
    retString.concat(rChar);

  }
  return retString;
}
void EEPROMWritelong(int address, long value)
{
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}
long EEPROMReadlong(long address)
{
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}





String GetMacAddress()
{
  uint8_t mac[6];
  char macStr[18] = {0};
  WiFi.macAddress(mac);
  sprintf(macStr, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],  mac[1], mac[2], mac[3], mac[4], mac[5]);
  return  String(macStr);
}

// convert a single hex digit character to its integer value (from https://code.google.com/p/avr-netino/)
unsigned char h2int(char c)
{
  if (c >= '0' && c <= '9') {
    return ((unsigned char)c - '0');
  }
  if (c >= 'a' && c <= 'f') {
    return ((unsigned char)c - 'a' + 10);
  }
  if (c >= 'A' && c <= 'F') {
    return ((unsigned char)c - 'A' + 10);
  }
  return (0);
}

String urldecode(String input) // (based on https://code.google.com/p/avr-netino/)
{
  char c;
  String ret = "";

  for (byte t = 0; t < input.length(); t++)
  {
    c = input[t];
    if (c == '+') c = ' ';
    if (c == '%') {


      t++;
      c = input[t];
      t++;
      c = (h2int(c) << 4) | h2int(input[t]);
    }

    ret.concat(c);
  }
  return ret;

}


//---------------- Custom ------------------------

#ifdef ESP_12
  //ESP12E
  #define LED_RED 13
  #define LED_GREEN 12
#else
  // NodeMCU
  #define LED_RED D7
  #define LED_GREEN D6
#endif


#define OFF 0

enum ledColor {
  off,
  green,
  red,
  both
};

void led(ledColor color) {

  switch (color) {
    case green:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, HIGH);
      break;
    case red:
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_RED, LOW);
      break;
    case both:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, LOW);
      break;
    case off:
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_RED, HIGH);
      break;
    default:
      break;
  }
}


#endif
//---------------------------------------
#ifndef GLOBAL_H
#define GLOBAL_H


ESP8266WebServer server(80);                  // The Webserver
boolean firstStart = true;                    // On firststart = true, NTP will try to get a valid time
int AdminTimeOutCounter = 0;                  // Counter for Disabling the AdminMode
WiFiUDP UDPNTPClient;                     // NTP Client
volatile unsigned long UnixTimestamp = 0;               // GLOBALTIME  ( Will be set by NTP)
boolean Refresh = false; // For Main Loop, to refresh things like GPIO / WS2812
int cNTP_Update = 0;                      // Counter for Updating the time via NTP
Ticker tkSecond;                        // Second - Timer for Updating Datetime Structure
boolean AdminEnabled = true;    // Enable Admin Mode for a given Time

#define ACCESS_POINT_NAME  "ESP"
//#define ACCESS_POINT_PASSWORD  "12345678"
#define AdminTimeOut 1  // Defines the Time in Seconds, when the Admin-Mode will be diabled

#define MAX_CONNECTIONS 3




//custom declarations
int freq = -1; // signal off

#ifdef ESP_12
//ESP-12E
#define BEEPPIN 4
#define LEFTPIN 16
#define RIGHTPIN 14
#else
//NodeMCU
#define BEEPPIN D8
#define LEFTPIN D5
#define RIGHTPIN D4
#endif

int counter = 0;

#define LOOP_FAST 60 * 1000
#define LOOP_SLOW 120 * 1000
#define BEEPTICKER 100
char serverTransport[] = "transport.opendata.ch";
String url;
const int httpPort = 80;
const int intensity[] = {1, 4, 10, 20, 20, 40, 40, 80, 80, 160, 160, 160};
unsigned long waitLoopEntry, loopTime = LOOP_SLOW, waitJSONLoopEntry;
bool okNTPvalue = false;  // NTP signal ok
bool requestOK = false;
int minTillDep = -999, secTillDep, lastMinute;
ledColor ledColor;
boolean ledState = false;
unsigned long  ledCounter;
char str[80];
long departureTime, absoluteActualTime, actualTime;
String JSONline;
long departureTimeStamp, lastDepartureTimeStamp, customWatchdog;



int beepOffTimer, beepOnTimer, beepOffTime, beepOnTime ;

enum defDirection {
  none,
  left,
  right
};

enum defBeeper {
  beeperOn,
  beeperOff,
  beeperIdle
};

volatile defBeeper beeperStatus = beeperIdle;

enum defStatus {
  admin,
  idle,
  requestLeft,
  requestRight,
  recovery
};

defStatus status, lastStatus;


struct strConfig {
  String ssid;
  String password;
  byte  IP[4];
  byte  Netmask[4];
  byte  Gateway[4];
  boolean dhcp;
  String ntpServerName;
  long Update_Time_Via_NTP_Every;
  long timeZone;
  boolean isDayLightSaving;
  String DeviceName;
  byte wayToStation;
  byte warningBegin;
  String base;
  String right;
  String left;
} config;

byte currentDirection;
defStatus _lastStatus;



/*
**
** CONFIGURATION HANDLING
**
*/
void ConfigureWifi()
{
  Serial.println("Configuring Wifi");

  WiFi.begin ("WLAN", "password");

  WiFi.begin (config.ssid.c_str(), config.password.c_str());

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected");
    led(red);
    delay(500);
  }
  if (!config.dhcp)
  {
    WiFi.config(IPAddress(config.IP[0], config.IP[1], config.IP[2], config.IP[3] ),  IPAddress(config.Gateway[0], config.Gateway[1], config.Gateway[2], config.Gateway[3] ) , IPAddress(config.Netmask[0], config.Netmask[1], config.Netmask[2], config.Netmask[3] ));
  }
}


void WriteConfig()
{

  Serial.println("Writing Config");
  EEPROM.write(0, 'C');
  EEPROM.write(1, 'F');
  EEPROM.write(2, 'G');

  EEPROM.write(16, config.dhcp);
  EEPROM.write(17, config.isDayLightSaving);

  EEPROMWritelong(18, config.Update_Time_Via_NTP_Every); // 4 Byte
  EEPROMWritelong(22, config.timeZone); // 4 Byte

  EEPROM.write(32, config.IP[0]);
  EEPROM.write(33, config.IP[1]);
  EEPROM.write(34, config.IP[2]);
  EEPROM.write(35, config.IP[3]);

  EEPROM.write(36, config.Netmask[0]);
  EEPROM.write(37, config.Netmask[1]);
  EEPROM.write(38, config.Netmask[2]);
  EEPROM.write(39, config.Netmask[3]);

  EEPROM.write(40, config.Gateway[0]);
  EEPROM.write(41, config.Gateway[1]);
  EEPROM.write(42, config.Gateway[2]);
  EEPROM.write(43, config.Gateway[3]);

  WriteStringToEEPROM(64, config.ssid);
  WriteStringToEEPROM(96, config.password);
  WriteStringToEEPROM(128, config.ntpServerName);

  // Application Settings
  WriteStringToEEPROM(160, config.base);
  WriteStringToEEPROM(192, config.left);
  WriteStringToEEPROM(224, config.right);
  EEPROM.write(256, config.warningBegin);
  EEPROM.write(257, config.wayToStation);

  WriteStringToEEPROM(258, config.DeviceName);

  EEPROM.commit();
}
boolean ReadConfig()
{
  Serial.println("Reading Configuration");
  if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F'  && EEPROM.read(2) == 'G' )
  {
    Serial.println("Configurarion Found!");
    config.dhcp =   EEPROM.read(16);

    config.isDayLightSaving = EEPROM.read(17);

    config.Update_Time_Via_NTP_Every = EEPROMReadlong(18); // 4 Byte

    config.timeZone = EEPROMReadlong(22); // 4 Byte

    config.IP[0] = EEPROM.read(32);
    config.IP[1] = EEPROM.read(33);
    config.IP[2] = EEPROM.read(34);
    config.IP[3] = EEPROM.read(35);
    config.Netmask[0] = EEPROM.read(36);
    config.Netmask[1] = EEPROM.read(37);
    config.Netmask[2] = EEPROM.read(38);
    config.Netmask[3] = EEPROM.read(39);
    config.Gateway[0] = EEPROM.read(40);
    config.Gateway[1] = EEPROM.read(41);
    config.Gateway[2] = EEPROM.read(42);
    config.Gateway[3] = EEPROM.read(43);
    config.ssid = ReadStringFromEEPROM(64);
    config.password = ReadStringFromEEPROM(96);
    config.ntpServerName = ReadStringFromEEPROM(128);


    // Application parameters
    config.base = ReadStringFromEEPROM(160);
    config.left = ReadStringFromEEPROM(192);
    config.right = ReadStringFromEEPROM(224);
    config.warningBegin = EEPROM.read(256);
    config.wayToStation = EEPROM.read(257);

    config.DeviceName = ReadStringFromEEPROM(258);
    return true;

  }
  else
  {
    Serial.println("Configurarion NOT FOUND!!!!");
    return false;
  }
}



#endif

//---------------------------------------

/*
**
**  NTP
**
*/


static const uint8_t monthDays[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
#define LEAP_YEAR(Y) ( ((1970+Y)>0) && !((1970+Y)%4) && ( ((1970+Y)%100) || !((1970+Y)%400) ) )


struct  strDateTime
{
  byte hour;
  byte minute;
  byte second;
  int year;
  byte month;
  byte day;
  byte wday;
} ;

strDateTime DateTime;                      // Global DateTime structure, will be refreshed every Second
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[ NTP_PACKET_SIZE];



void storeNTPtime()
{
  unsigned long _unixTime = 0;

  if (WiFi.status() == WL_CONNECTED)
  {
    UDPNTPClient.begin(2390);  // Port for NTP receive
    IPAddress timeServerIP;
    WiFi.hostByName(config.ntpServerName.c_str(), timeServerIP);

    //Serial.println("sending NTP packet...");
    memset(packetBuffer, 0, NTP_PACKET_SIZE);
    packetBuffer[0] = 0b11100011;   // LI, Version, Mode
    packetBuffer[1] = 0;     // Stratum, or type of clock
    packetBuffer[2] = 6;     // Polling Interval
    packetBuffer[3] = 0xEC;  // Peer Clock Precision
    packetBuffer[12]  = 49;
    packetBuffer[13]  = 0x4E;
    packetBuffer[14]  = 49;
    packetBuffer[15]  = 52;
    UDPNTPClient.beginPacket(timeServerIP, 123);
    UDPNTPClient.write(packetBuffer, NTP_PACKET_SIZE);
    UDPNTPClient.endPacket();

    delay(100);

    int cb = UDPNTPClient.parsePacket();
    if (cb == 0) {
      Serial.println("No NTP packet yet");
    }
    else
    {
      Serial.print("NTP packet received, length=");
      Serial.println(cb);
      UDPNTPClient.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      const unsigned long seventyYears = 2208988800UL;
      _unixTime = secsSince1900 - seventyYears;

    }
  } else {
    Serial.println("Internet yet not connected");
    delay(500);
  }
  yield();
  if (_unixTime > 0) UnixTimestamp = _unixTime; // store universally available time stamp
}


strDateTime ConvertUnixTimeStamp( unsigned long _tempTimeStamp) {
  strDateTime _tempDateTime;
  uint8_t year;
  uint8_t month, monthLength;
  uint32_t time;
  unsigned long days;

  time = (uint32_t)_tempTimeStamp;
  _tempDateTime.second = time % 60;
  time /= 60; // now it is minutes
  _tempDateTime.minute = time % 60;
  time /= 60; // now it is hours
  _tempDateTime.hour = time % 24;
  time /= 24; // now it is days
  _tempDateTime.wday = ((time + 4) % 7) + 1;  // Sunday is day 1

  year = 0;
  days = 0;
  while ((unsigned)(days += (LEAP_YEAR(year) ? 366 : 365)) <= time) {
    year++;
  }
  _tempDateTime.year = year; // year is offset from 1970

  days -= LEAP_YEAR(year) ? 366 : 365;
  time  -= days; // now it is days in this year, starting at 0

  days = 0;
  month = 0;
  monthLength = 0;
  for (month = 0; month < 12; month++) {
    if (month == 1) { // february
      if (LEAP_YEAR(year)) {
        monthLength = 29;
      } else {
        monthLength = 28;
      }
    } else {
      monthLength = monthDays[month];
    }

    if (time >= monthLength) {
      time -= monthLength;
    } else {
      break;
    }
  }
  _tempDateTime.month = month + 1;  // jan is month 1
  _tempDateTime.day = time + 1;     // day of month
  _tempDateTime.year += 1970;

  return _tempDateTime;
}


//
// Summertime calculates the daylight saving time for middle Europe. Input: Unixtime in UTC
//
boolean summerTime(unsigned long _timeStamp ) {
  strDateTime  _tempDateTime = ConvertUnixTimeStamp(_timeStamp);
  // printTime("Innerhalb ", _tempDateTime);

  if (_tempDateTime.month < 3 || _tempDateTime.month > 10) return false; // keine Sommerzeit in Jan, Feb, Nov, Dez
  if (_tempDateTime.month > 3 && _tempDateTime.month < 10) return true; // Sommerzeit in Apr, Mai, Jun, Jul, Aug, Sep
  if (_tempDateTime.month == 3 && (_tempDateTime.hour + 24 * _tempDateTime.day) >= (3 +  24 * (31 - (5 * _tempDateTime.year / 4 + 4) % 7)) || _tempDateTime.month == 10 && (_tempDateTime.hour + 24 * _tempDateTime.day) < (3 +  24 * (31 - (5 * _tempDateTime.year / 4 + 1) % 7)))
    return true;
  else
    return false;
}

unsigned long adjustTimeZone(unsigned long _timeStamp, int _timeZone, bool _isDayLightSavingSaving) {
  strDateTime _tempDateTime;
  _timeStamp += _timeZone *  360; // adjust timezone
  // printTime("Innerhalb adjustTimeZone ", ConvertUnixTimeStamp(_timeStamp));
  if (_isDayLightSavingSaving && summerTime(_timeStamp)) _timeStamp += 3600; // Sommerzeit beachten
  return _timeStamp;
}


void ISRsecondTick()
{
  strDateTime _tempDateTime;
  AdminTimeOutCounter++;
  cNTP_Update++;
  UnixTimestamp++;
  absoluteActualTime = adjustTimeZone(UnixTimestamp, config.timeZone, config.isDayLightSaving);
  DateTime = ConvertUnixTimeStamp(absoluteActualTime);  //  convert to DateTime format
  actualTime = 3600 * DateTime.hour + 60 * DateTime.minute + DateTime.second;
  if (millis() - customWatchdog > 30000){
    Serial.println("CustomWatchdog bites. Bye");
    ESP.reset();
  }
}



// Incluye las "Páginas" HTML, STYLE y Script

const char PAGE_Root[] PROGMEM = R"=====(
<!doctype html>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<script src="microajax.js"></script> 
<html>
  <head>
  <title></title>
  <meta charset="utf-8" />
  </head>
  <body>
  <link rel="stylesheet" href="style.css" type="text/css" />
  It work's!
  </body>
</html>

)=====";

void sendRootPage()
{        
    if (server.args() > 0 )  // Are there any POST/GET Fields ? 
    {
       for ( uint8_t i = 0; i < server.args(); i++ ) {  // Iterate through the fields
            
        }
    }
    server.send ( 200, "text/html", PAGE_Root ); 
}
 

//----------------------------

const char PAGE_Root[] PROGMEM = R"=====(
<!doctype html>
<meta name="viewport" content="width=device-width, initial-scale=1" />
<script src="microajax.js"></script> 
<html>
  <head>
  <title></title>
  <meta charset="utf-8" />
  </head>
  <body>
  <link rel="stylesheet" href="style.css" type="text/css" />
  It work's!
  </body>
</html>

)=====";

void sendRootPage()
{        
    if (server.args() > 0 )  // Are there any POST/GET Fields ? 
    {
       for ( uint8_t i = 0; i < server.args(); i++ ) {  // Iterate through the fields
            
        }
    }
    server.send ( 200, "text/html", PAGE_Root ); 
}
 

//-----------------------------


//
//  HTML PAGE
//

const char PAGE_AdminMainPage[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<strong>Administration</strong>
<hr>
<a href="general.html" style="width:250px" class="btn btn--m btn--blue" >General Configuration</a><br>
<a href="config.html" style="width:250px" class="btn btn--m btn--blue" >Network Configuration</a><br>
<a href="info.html"   style="width:250px"  class="btn btn--m btn--blue" >Network Information</a><br>
<a href="ntp.html"   style="width:250px"  class="btn btn--m btn--blue" >NTP Settings</a><br>
<a href="appl.html"   style="width:250px"  class="btn btn--m btn--blue" >Application Settings</a><br>


<script>
window.onload = function ()
{
  load("style.css","css", function() 
  {
    load("microajax.js","js", function() 
    {
        // Do something after load...
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>

)=====";

//-----------------------------





const char PAGE_microajax_js[] PROGMEM = R"=====(
function microAjax(B,A){this.bindFunction=function(E,D){return function(){return E.apply(D,[D])}};this.stateChange=function(D){if(this.request.readyState==4){this.callbackFunction(this.request.responseText)}};this.getRequest=function(){if(window.ActiveXObject){return new ActiveXObject("Microsoft.XMLHTTP")}else{if(window.XMLHttpRequest){return new XMLHttpRequest()}}return false};this.postBody=(arguments[2]||"");this.callbackFunction=A;this.url=B;this.request=this.getRequest();if(this.request){var C=this.request;C.onreadystatechange=this.bindFunction(this.stateChange,this);if(this.postBody!==""){C.open("POST",B,true);C.setRequestHeader("X-Requested-With","XMLHttpRequest");C.setRequestHeader("Content-type","application/x-www-form-urlencoded");C.setRequestHeader("Connection","close")}else{C.open("GET",B,true)}C.send(this.postBody)}};

function setValues(url)
{
  microAjax(url, function (res)
  {
    res.split(String.fromCharCode(10)).forEach(function(entry) {
    fields = entry.split("|");
    if(fields[2] == "input")
    {
        document.getElementById(fields[0]).value = fields[1];
    }
    else if(fields[2] == "div")
    {
        document.getElementById(fields[0]).innerHTML  = fields[1];
    }
    else if(fields[2] == "chk")
    {
        document.getElementById(fields[0]).checked  = fields[1];
    }
    });
  });
}

)=====";

//-----------------------------


const char PAGE_Style_css[] PROGMEM = R"=====(
body { color: #000000; font-family: avenir, helvetica, arial, sans-serif;  letter-spacing: 0.15em;} 
hr {    background-color: #eee;    border: 0 none;   color: #eee;    height: 1px; } 
.btn, .btn:link, .btn:visited {  
  border-radius: 0.3em;  
  border-style: solid;  
  border-width: 1px;  
color: #111;  
display: inline-block;  
  font-family: avenir, helvetica, arial, sans-serif;  
  letter-spacing: 0.15em;  
  margin-bottom: 0.5em;  
padding: 1em 0.75em;  
  text-decoration: none;  
  text-transform: uppercase;  
  -webkit-transition: color 0.4s, background-color 0.4s, border 0.4s;  
transition: color 0.4s, background-color 0.4s, border 0.4s; 
} 
.btn:hover, .btn:focus {
color: #7FDBFF;  
border: 1px solid #7FDBFF;  
  -webkit-transition: background-color 0.3s, color 0.3s, border 0.3s;  
transition: background-color 0.3s, color 0.3s, border 0.3s; 
}
  .btn:active {  
color: #0074D9;  
border: 1px solid #0074D9;  
    -webkit-transition: background-color 0.3s, color 0.3s, border 0.3s;  
transition: background-color 0.3s, color 0.3s, border 0.3s; 
  } 
  .btn--s 
  {  
    font-size: 12px; 
  }
  .btn--m { 
    font-size: 14px; 
  }
  .btn--l {  
    font-size: 20px;  border-radius: 0.25em !important; 
  } 
  .btn--full, .btn--full:link {
    border-radius: 0.25em; 
display: block;  
      margin-left: auto; 
      margin-right: auto; 
      text-align: center; 
width: 100%; 
  } 
  .btn--blue:link, .btn--blue:visited {
color: #fff;  
    background-color: #0074D9; 
  }
  .btn--blue:hover, .btn--blue:focus {
color: #fff !important;  
    background-color: #0063aa;  
    border-color: #0063aa; 
  }
  .btn--blue:active {
color: #fff; 
    background-color: #001F3F;  border-color: #001F3F; 
  }
  @media screen and (min-width: 32em) {
    .btn--full {  
      max-width: 16em !important; } 
  }
)=====";
 
//------------------------------------

const char PAGE_NTPConfiguration[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>NTP Settings</strong>
<hr>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr><td align="right">NTP Server:</td><td><input type="text" id="ntpserver" name="ntpserver" maxlength="172" value=""></td></tr>
<tr><td align="right">Update:</td><td><input type="text" id="update" name="update" size="3"maxlength="6" value=""> minutes (0=disable)</td></tr>
<tr><td>Timezone</td><td>
<select  id="tz" name="tz">
  <option value="-120">(GMT-12:00)</option>
  <option value="-110">(GMT-11:00)</option>
  <option value="-100">(GMT-10:00)</option>
  <option value="-90">(GMT-09:00)</option>
  <option value="-80">(GMT-08:00)</option>
  <option value="-70">(GMT-07:00)</option>
  <option value="-60">(GMT-06:00)</option>
  <option value="-50">(GMT-05:00)</option>
  <option value="-40">(GMT-04:00)</option>
  <option value="-35">(GMT-03:30)</option>
  <option value="-30">(GMT-03:00)</option>
  <option value="-20">(GMT-02:00)</option>
  <option value="-10">(GMT-01:00)</option>
  <option value="0">(GMT+00:00)</option>
  <option value="10">(GMT+01:00)</option>
  <option value="20">(GMT+02:00)</option>
  <option value="30">(GMT+03:00)</option>
  <option value="35">(GMT+03:30)</option>
  <option value="40">(GMT+04:00)</option>
  <option value="45">(GMT+04:30)</option>
  <option value="50">(GMT+05:00)</option>
  <option value="55">(GMT+05:30)</option>
  <option value="57">(GMT+05:45)</option>
  <option value="60">(GMT+06:00)</option>
  <option value="65">(GMT+06:30)</option>
  <option value="70">(GMT+07:00)</option>
  <option value="80">(GMT+08:00)</option>
  <option value="90">(GMT+09:00)</option>
  <option value="95">(GMT+09:30)</option>
  <option value="100">(GMT+10:00)</option>
  <option value="110">(GMT+11:00)</option>
  <option value="120">(GMT+12:00)</option>
  <option value="120">(GMT+12:00)</option>
  <option value="130">(GMT+13:00)</option>
</select>
</td></tr>
<tr><td align="right">Daylight saving:</td><td><input type="checkbox" id="dst" name="dst"></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<script>
  

window.onload = function ()
{
  load("style.css","css", function() 
  {
    load("microajax.js","js", function() 
    {
        setValues("/admin/ntpvalues");
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


void send_NTP_configuration_html()
{
  if (server.args() > 0 )  // Save Settings
  {
    config.isDayLightSaving = false;
    String temp = "";
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "ntpserver") config.ntpServerName = urldecode( server.arg(i)); 
      if (server.argName(i) == "update") config.Update_Time_Via_NTP_Every =  server.arg(i).toInt(); 
      if (server.argName(i) == "tz") config.timeZone =  server.arg(i).toInt(); 
      if (server.argName(i) == "dst") config.isDayLightSaving = true; 
    }
    WriteConfig();
   
    firstStart = true;
  }
  server.send ( 200, "text/html", PAGE_NTPConfiguration ); 
  Serial.println(__FUNCTION__); 
  
}



void send_NTP_configuration_values_html()
{
    
  String values ="";
  values += "ntpserver|" + (String) config.ntpServerName + "|input\n";
  values += "update|" +  (String) config.Update_Time_Via_NTP_Every + "|input\n";
  values += "tz|" +  (String) config.timeZone + "|input\n";
  values += "dst|" +  (String) (config.isDayLightSaving ? "checked" : "") + "|chk\n";
  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__); 
  AdminTimeOutCounter=0;
}

//------------------------------------
#ifndef PAGE_INFOMATION_H
#define PAGE_INFOMATION_H


//
//   The HTML PAGE
//
const char PAGE_Information[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<link rel="stylesheet" href="style.css" type="text/css" />
<script src="microajax.js"></script> 
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Network Information</strong>
<hr>
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">SSID :</td><td><span id="x_ssid"></span></td></tr>
<tr><td align="right">IP :</td><td><span id="x_ip"></span></td></tr>
<tr><td align="right">Netmask :</td><td><span id="x_netmask"></span></td></tr>
<tr><td align="right">Gateway :</td><td><span id="x_gateway"></span></td></tr>
<tr><td align="right">Mac :</td><td><span id="x_mac"></span></td></tr>


<tr><td colspan="2" align="center"><a href="javascript:GetState()" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>
<script>

function GetState()
{
  setValues("/admin/infovalues");
}

window.onload = function ()
{
  load("style.css","css", function() 
  {
    load("microajax.js","js", function() 
    {
        GetState();
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====" ;


//
// FILL WITH INFOMATION
// 

void send_information_values_html ()
{

  String values ="";

  values += "x_ssid|" + (String)WiFi.SSID() +  "|div\n";
  values += "x_ip|" +  (String) WiFi.localIP()[0] + "." +  (String) WiFi.localIP()[1] + "." +  (String) WiFi.localIP()[2] + "." + (String) WiFi.localIP()[3] +  "|div\n";
  values += "x_gateway|" +  (String) WiFi.gatewayIP()[0] + "." +  (String) WiFi.gatewayIP()[1] + "." +  (String) WiFi.gatewayIP()[2] + "." + (String) WiFi.gatewayIP()[3] +  "|div\n";
  values += "x_netmask|" +  (String) WiFi.subnetMask()[0] + "." +  (String) WiFi.subnetMask()[1] + "." +  (String) WiFi.subnetMask()[2] + "." + (String) WiFi.subnetMask()[3] +  "|div\n";
  values += "x_mac|" + GetMacAddress() +  "|div\n";
  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__); 
  AdminTimeOutCounter=0;
}


#endif

//------------------------------------
//------------------------------------
//
//  HTML PAGE
//

const char PAGE_AdminGeneralSettings[] PROGMEM =  R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>General Settings</strong>
<hr>
<form action="" method="post">
<table border="0"  cellspacing="0" cellpadding="3" >
<tr>
  <td align="right">Name of Device</td>
  <td><input type="text" id="devicename" name="devicename" value=""></td>
</tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<script>

 

window.onload = function ()
{
  load("style.css","css", function() 
  {
    load("microajax.js","js", function() 
    {
        setValues("/admin/generalvalues");
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



</script>
)=====";


// Functions for this Page
void send_devicename_value_html()
{
    
  String values ="";
  values += "devicename|" + (String) config.DeviceName + "|div\n";
  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__); 
  
}

void send_general_html()
{
  
  if (server.args() > 0 )  // Save Settings
  { 
    String temp = "";
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "devicename") config.DeviceName = urldecode(server.arg(i)); 
    }
    WriteConfig();
    firstStart = true;
  }
  server.send ( 200, "text/html", PAGE_AdminGeneralSettings ); 
  Serial.println(__FUNCTION__); 
  
  
}

void send_general_configuration_values_html()
{
  String values ="";
  values += "devicename|" +  (String)  config.DeviceName +  "|input\n";
 
  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__); 
  AdminTimeOutCounter=0;
}

//------------------------------------
//------------------------------------
//
//  HTML PAGE
//
const char PAGE_ApplicationConfiguration[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Application Configuration</strong>
<hr>
Connect to Router with these settings:<br>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:350px" >
<tr><td align="right">Home:</td><td><input type="text" id="base" name="base" value=""></td></tr>
<tr><td align="right">Left:</td><td><input type="text" id="left" name="left" value=""></td></tr>
<tr><td align="right">Right:</td><td><input type="text" id="right" name="right" value=""></td></tr>
<tr><td align="right">Way to station (min):</td><td><input type="text" id="wayToStation" name="wayToStation" value=""></td></tr>
<tr><td align="right">Warning starts (min):</td><td><input type="text" id="warningBegin" name="warningBegin" value=""></td></tr>

<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>

<script>

function GetState()
{
  setValues("/admin/applvalues");
}


window.onload = function ()
{
  load("style.css","css", function() 
  {
    load("microajax.js","js", function() 
    {
          setValues("/admin/applvalues");

    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}

</script>


)=====";

//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void send_application_configuration_html()
{
  if (server.args() > 0 )  // Save Settings
  {
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "base") config.base =   urldecode(server.arg(i));
      if (server.argName(i) == "left") config.left =    urldecode(server.arg(i));
      if (server.argName(i) == "right") config.right =    urldecode(server.arg(i));
      if (server.argName(i) == "wayToStation") config.wayToStation =  urldecode(server.arg(i)).toInt();
      if (server.argName(i) == "warningBegin") config.warningBegin =  urldecode(server.arg(i)).toInt();
      }
    if (config.wayToStation>20) config.wayToStation=20;
    if (config.wayToStation<0) config.wayToStation=0;

    if (config.warningBegin>10) config.warningBegin=10;
    if (config.warningBegin<0) config.warningBegin=0;
    
    WriteConfig();
  }
   server.send ( 200, "text/html", PAGE_ApplicationConfiguration ); 
  Serial.println(__FUNCTION__); 
}



//
//   FILL THE PAGE WITH VALUES
//

void send_application_configuration_values_html()
{

  String values ="";

  values += "base|" + (String) config.base + "|input\n";
  values += "left|" +  (String) config.left + "|input\n";
  values += "right|" +  (String) config.right + "|input\n";
  values += "wayToStation|" +  (String) config.wayToStation + "|input\n";
   values += "warningBegin|" +  (String) config.warningBegin + "|input\n";
  
  server.send ( 200, "text/plain", values);
  Serial.print("1 ");
  Serial.println(__FUNCTION__); 
  AdminTimeOutCounter=0;
  
}

//------------------------------------
//------------------------------------


//
//  HTML PAGE
//
const char PAGE_NetworkConfiguration[] PROGMEM = R"=====(
<meta name="viewport" content="width=device-width, initial-scale=1" />
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<a href="/"  class="btn btn--s"><</a>&nbsp;&nbsp;<strong>Network Configuration</strong>
<hr>
Connect to Router with these settings:<br>
<form action="" method="get">
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >
<tr><td align="right">SSID:</td><td><input type="text" id="ssid" name="ssid" value=""></td></tr>
<tr><td align="right">Password:</td><td><input type="text" id="password" name="password" value=""></td></tr>
<tr><td align="right">DHCP:</td><td><input type="checkbox" id="dhcp" name="dhcp"></td></tr>
<tr><td align="right">IP:     </td><td><input type="text" id="ip_0" name="ip_0" size="3">.<input type="text" id="ip_1" name="ip_1" size="3">.<input type="text" id="ip_2" name="ip_2" size="3">.<input type="text" id="ip_3" name="ip_3" value="" size="3"></td></tr>
<tr><td align="right">Netmask:</td><td><input type="text" id="nm_0" name="nm_0" size="3">.<input type="text" id="nm_1" name="nm_1" size="3">.<input type="text" id="nm_2" name="nm_2" size="3">.<input type="text" id="nm_3" name="nm_3" size="3"></td></tr>
<tr><td align="right">Gateway:</td><td><input type="text" id="gw_0" name="gw_0" size="3">.<input type="text" id="gw_1" name="gw_1" size="3">.<input type="text" id="gw_2" name="gw_2" size="3">.<input type="text" id="gw_3" name="gw_3" size="3"></td></tr>
<tr><td colspan="2" align="center"><input type="submit" style="width:150px" class="btn btn--m btn--blue" value="Save"></td></tr>
</table>
</form>
<hr>
<strong>Connection State:</strong><div id="connectionstate">N/A</div>
<hr>
<strong>Networks:</strong><br>
<table border="0"  cellspacing="3" style="width:310px" >
<tr><td><div id="networks">Scanning...</div></td></tr>
<tr><td align="center"><a href="javascript:GetState()" style="width:150px" class="btn btn--m btn--blue">Refresh</a></td></tr>
</table>


<script>

function GetState()
{
  setValues("/admin/connectionstate");
}
function selssid(value)
{
  document.getElementById("ssid").value = value; 
}


window.onload = function ()
{
  load("style.css","css", function() 
  {
    load("microajax.js","js", function() 
    {
          setValues("/admin/values");
          setTimeout(GetState,3000);
    });
  });
}
function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}




</script>


)=====";

const char PAGE_WaitAndReload[] PROGMEM = R"=====(
<meta http-equiv="refresh" content="5; URL=config.html">
Please Wait....Configuring and Restarting.
)=====";


//
//  SEND HTML PAGE OR IF A FORM SUMBITTED VALUES, PROCESS THESE VALUES
// 

void send_network_configuration_html()
{
  
  if (server.args() > 0 )  // Save Settings
  {
    String temp = "";
    config.dhcp = false;
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      if (server.argName(i) == "ssid") config.ssid =   urldecode(server.arg(i));
      if (server.argName(i) == "password") config.password =    urldecode(server.arg(i));
      if (server.argName(i) == "ip_0") if (checkRange(server.arg(i)))   config.IP[0] =  server.arg(i).toInt();
      if (server.argName(i) == "ip_1") if (checkRange(server.arg(i)))   config.IP[1] =  server.arg(i).toInt();
      if (server.argName(i) == "ip_2") if (checkRange(server.arg(i)))   config.IP[2] =  server.arg(i).toInt();
      if (server.argName(i) == "ip_3") if (checkRange(server.arg(i)))   config.IP[3] =  server.arg(i).toInt();
      if (server.argName(i) == "nm_0") if (checkRange(server.arg(i)))   config.Netmask[0] =  server.arg(i).toInt();
      if (server.argName(i) == "nm_1") if (checkRange(server.arg(i)))   config.Netmask[1] =  server.arg(i).toInt();
      if (server.argName(i) == "nm_2") if (checkRange(server.arg(i)))   config.Netmask[2] =  server.arg(i).toInt();
      if (server.argName(i) == "nm_3") if (checkRange(server.arg(i)))   config.Netmask[3] =  server.arg(i).toInt();
      if (server.argName(i) == "gw_0") if (checkRange(server.arg(i)))   config.Gateway[0] =  server.arg(i).toInt();
      if (server.argName(i) == "gw_1") if (checkRange(server.arg(i)))   config.Gateway[1] =  server.arg(i).toInt();
      if (server.argName(i) == "gw_2") if (checkRange(server.arg(i)))   config.Gateway[2] =  server.arg(i).toInt();
      if (server.argName(i) == "gw_3") if (checkRange(server.arg(i)))   config.Gateway[3] =  server.arg(i).toInt();
      if (server.argName(i) == "dhcp") config.dhcp = true;
    }
     server.send ( 200, "text/html", PAGE_WaitAndReload );
    WriteConfig();
    ConfigureWifi();    
  }
  else
  {
    server.send ( 200, "text/html", PAGE_NetworkConfiguration ); 
  }
  Serial.println(__FUNCTION__); 
}



//
//   FILL THE PAGE WITH VALUES
//

void send_network_configuration_values_html()
{

  String values ="";

  values += "ssid|" + (String) config.ssid + "|input\n";
  values += "password|" +  (String) config.password + "|input\n";
  values += "ip_0|" +  (String) config.IP[0] + "|input\n";
  values += "ip_1|" +  (String) config.IP[1] + "|input\n";
  values += "ip_2|" +  (String) config.IP[2] + "|input\n";
  values += "ip_3|" +  (String) config.IP[3] + "|input\n";
  values += "nm_0|" +  (String) config.Netmask[0] + "|input\n";
  values += "nm_1|" +  (String) config.Netmask[1] + "|input\n";
  values += "nm_2|" +  (String) config.Netmask[2] + "|input\n";
  values += "nm_3|" +  (String) config.Netmask[3] + "|input\n";
  values += "gw_0|" +  (String) config.Gateway[0] + "|input\n";
  values += "gw_1|" +  (String) config.Gateway[1] + "|input\n";
  values += "gw_2|" +  (String) config.Gateway[2] + "|input\n";
  values += "gw_3|" +  (String) config.Gateway[3] + "|input\n";
  values += "dhcp|" +  (String) (config.dhcp ? "checked" : "") + "|chk\n";
  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__); 
  
}


//
//   FILL THE PAGE WITH NETWORKSTATE & NETWORKS
//

void send_connection_state_values_html()
{

  String state = "N/A";
  String Networks = "";
  if (WiFi.status() == 0) state = "Idle";
  else if (WiFi.status() == 1) state = "NO SSID AVAILBLE";
  else if (WiFi.status() == 2) state = "SCAN COMPLETED";
  else if (WiFi.status() == 3) state = "CONNECTED";
  else if (WiFi.status() == 4) state = "CONNECT FAILED";
  else if (WiFi.status() == 5) state = "CONNECTION LOST";
  else if (WiFi.status() == 6) state = "DISCONNECTED";



   int n = WiFi.scanNetworks();

   if (n == 0)
   {
     Networks = "<font color='#FF0000'>No networks found!</font>";
   }
  else
    {
   
    
    Networks = "Found " +String(n) + " Networks<br>";
    Networks += "<table border='0' cellspacing='0' cellpadding='3'>";
    Networks += "<tr bgcolor='#DDDDDD' ><td><strong>Name</strong></td><td><strong>Quality</strong></td><td><strong>Enc</strong></td><tr>";
    for (int i = 0; i < n; ++i)
    {
      int quality=0;
      if(WiFi.RSSI(i) <= -100)
      {
          quality = 0;
      }
      else if(WiFi.RSSI(i) >= -50)
      {
          quality = 100;
      }
      else
      {
        quality = 2 * (WiFi.RSSI(i) + 100);
      }


      Networks += "<tr><td><a href='javascript:selssid(\""  +  String(WiFi.SSID(i))  + "\")'>"  +  String(WiFi.SSID(i))  + "</a></td><td>" +  String(quality) + "%</td><td>" +  String((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*")  + "</td></tr>";
    }
    Networks += "</table>";
  }
   
  String values ="";
  values += "connectionstate|" +  state + "|div\n";
  values += "networks|" +  Networks + "|div\n";
  server.send ( 200, "text/plain", values);
  Serial.println(__FUNCTION__); 
  AdminTimeOutCounter=0;
}

//------------------------------------
#ifndef PAGE_EXAMPLE_H
#define PAGE_EXAMPLE_H
//
//   The EXAMPLE PAGE
//
const char PAGE_example[] PROGMEM = R"=====(
  <meta name="viewport" content="width=device-width, initial-scale=1" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <h1>My Example goes here</h1>     
    
   <div id="mydynamicdata">Here comes the Dynamic Data in </div>   <!-- added a DIV, where the dynamic data goes to -->
   <script>                


    window.onload = function ()
    {
      load("style.css","css", function() 
      {
        load("microajax.js","js", function() 
        {
            setValues("/admin/filldynamicdata");  //-- this function calls the function on the ESP      
        });
      });
    }
    function load(e,t,n){if("js"==t){var a=document.createElement("script");a.src=e,a.type="text/javascript",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}else if("css"==t){var a=document.createElement("link");a.href=e,a.rel="stylesheet",a.type="text/css",a.async=!1,a.onload=function(){n()},document.getElementsByTagName("head")[0].appendChild(a)}}



   </script>

)=====";
#endif


void filldynamicdata()
{        
    String values ="";
    values += "mydynamicdata|" + (String) + "This is filled by AJAX. Millis since start: " + (String) millis() + "|div\n";   // Build a string, like this:  ID|VALUE|TYPE
    server.send ( 200, "text/plain", values);   
}

void processExample()
{        
    if (server.args() > 0 )  // Are there any POST/GET Fields ? 
    {
       for ( uint8_t i = 0; i < server.args(); i++ ) {  // Iterate through the fields
            if (server.argName(i) == "firstname") 
            {
                 // Your processing for the transmitted form-variable 
                 String fName = server.arg(i);
            }
        }
    }
    server.send ( 200, "text/html", PAGE_example  ); 
}

//------------------------------------

extern "C" {
#include "user_interface.h"
}
WiFiClient client;
Ticker ticker;

os_timer_t myTimer;


//OTA
const char* host = "esp8266-ota";
const uint16_t aport = 8266;
bool otaFlag = false;
WiFiServer TelnetServer(aport);
WiFiClient Telnet;
WiFiUDP OTA;

void setup ( void ) {
  EEPROM.begin(512);
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Starting ESP8266");

  os_timer_setfn(&myTimer, ISRbeepTicker, NULL);
  os_timer_arm(&myTimer, BEEPTICKER, true);

  // Personalizado
  pinMode(BEEPPIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LEFTPIN, INPUT_PULLUP);
  pinMode(RIGHTPIN, INPUT_PULLUP);
  ledColor = off;
  beep(3);
  delay(2000);
  if (!ReadConfig())
  {
    // DEFAULT CONFIG
    Serial.println("Setting default parameters");
// por favor, defina las credenciales en el archivo credentials.h o aquí
    config.ssid = mySSID; // SSID of access point
    config.password = mypassword;  // password of access point
    config.dhcp = true;
    config.IP[0] = 192; config.IP[1] = 168; config.IP[2] = 1; config.IP[3] = 100;
    config.Netmask[0] = 255; config.Netmask[1] = 255; config.Netmask[2] = 255; config.Netmask[3] = 0;
    config.Gateway[0] = 192; config.Gateway[1] = 168; config.Gateway[2] = 1; config.Gateway[3] = 1;
    config.ntpServerName = "0.ch.pool.ntp.org";
    config.Update_Time_Via_NTP_Every =  5;
    config.timeZone = 1;
    config.isDayLightSaving = true;
    config.DeviceName = "Not Named";
    config.wayToStation = 3;
    config.warningBegin = 5;
    config.base = "lausen stutz";
    config.right = "lausen";
    config.left = "lausen";
    WriteConfig();
  }
  if (!(digitalRead(LEFTPIN) || digitalRead(RIGHTPIN))) {   // OTA Mode?
    Serial.println("OTA READY");
    otaFlag = true;
    otaInit();
    for (int i = 0; i < 10; i++) {
      ledColor = both;
      delay(200);
      ledColor = off;
      delay(200);
    }
  } else {
    // normal operation
    status = admin;
    tkSecond.attach(1, ISRsecondTick);

    currentDirection = EEPROM.read(300);
    Serial.printf("Current Direction %d \n", currentDirection);
    if ((currentDirection == left || currentDirection == right) && digitalRead(LEFTPIN)) {
      // ---------------- RECOVERY -----------------------
      status = recovery;
    } else {

      // normal operation
      WiFi.mode(WIFI_STA);
      WiFi.softAP( "ESP");

      // Admin page
      server.on ( "/", []() {
        Serial.println("admin.html");
        server.send ( 200, "text/html", PAGE_AdminMainPage );  // const char top of page
      }  );

      server.on ( "/favicon.ico",   []() {
        Serial.println("favicon.ico");
        server.send ( 200, "text/html", "" );
      }  );

      // Network config
      server.on ( "/config.html", send_network_configuration_html );
      // Info Page
      server.on ( "/info.html", []() {
        Serial.println("info.html");
        server.send ( 200, "text/html", PAGE_Information );
      }  );
      server.on ( "/ntp.html", send_NTP_configuration_html  );

      server.on ( "/appl.html", send_application_configuration_html  );
      server.on ( "/general.html", send_general_html  );
      //  server.on ( "/example.html", []() { server.send ( 200, "text/html", PAGE_EXAMPLE );  } );
      server.on ( "/style.css", []() {
        Serial.println("style.css");
        server.send ( 200, "text/plain", PAGE_Style_css );
      } );
      server.on ( "/microajax.js", []() {
        Serial.println("microajax.js");
        server.send ( 200, "text/plain", PAGE_microajax_js );
      } );
      server.on ( "/admin/values", send_network_configuration_values_html );
      server.on ( "/admin/connectionstate", send_connection_state_values_html );
      server.on ( "/admin/infovalues", send_information_values_html );
      server.on ( "/admin/ntpvalues", send_NTP_configuration_values_html );
      server.on ( "/admin/applvalues", send_application_configuration_values_html );
      server.on ( "/admin/generalvalues", send_general_configuration_values_html);
      server.on ( "/admin/devicename",     send_devicename_value_html);


      server.onNotFound ( []() {
        Serial.println("Page Not Found");
        server.send ( 400, "text/html", "Page not Found" );
      }  );
      server.begin();
      Serial.println( "HTTP server started" );

      AdminTimeOutCounter = 0;
      waitLoopEntry = millis();
    }
  }
}

void loop(void ) {
  yield(); // For ESP8266 to not dump

  if (otaFlag) {
    otaReceive();
  }
  else {
    customLoop();
  }
}

//-------------------------------------- CUSTOM ----------------------------------------

void customLoop() {
  defDirection _dir;
  String _line;

  // Non blocking code !!!
  switch (status) {
    case admin:
      ledColor = both;
      server.handleClient();

      // exit
      if (AdminTimeOutCounter > AdminTimeOut) {
        Serial.println("Admin Mode disabled!");
        ledColor = red;
        for (int hi = 0; hi < 3; hi++) beep(2);
        WiFi.mode(WIFI_AP);
        ConfigureWifi();
        ledColor = green;


        // exit
        waitJSONLoopEntry = 0;
        cNTP_Update = 999;
        status = idle;
        lastStatus = idle;
      }
      break;

    case idle:
      if (lastStatus != idle) Serial.println("Status idle");
      ledColor = off;
      storeDirToEEPROM(none);
      freq = -1;  // no signal
      url = "";
      JSONline = "";

      // exit
      _dir = readButton();
      if (_dir == left) status = requestLeft;
      if (_dir == right) status = requestRight;
      lastStatus = idle;
      break;



    case requestLeft:
      if (lastStatus != requestLeft) Serial.println("Status requestLeft");
      storeDirToEEPROM(left);
      url = "/v1/connections?from=" + config.base + "&to=" + config.left  + "&fields[]=connections/from/departure&fields[]=connections/from/prognosis/departure&fields[]=connections/from/departureTimestamp&limit=" + MAX_CONNECTIONS;
      if (lastStatus != requestLeft) storeDepartureString(); // if valid url
      if (JSONline.length() > 1) {
        processRequest();

        // exit
        if (lastDepartureTimeStamp != departureTimeStamp && (lastStatus == requestRight || lastStatus == requestLeft)) status = idle; // next departure time choosen
        //  Serial.printf("lastDepartureTimeStamp %d departureTimeStamp %d lastStatus %d \n", lastDepartureTimeStamp , departureTimeStamp, lastStatus);
        lastDepartureTimeStamp = departureTimeStamp;
        lastStatus = requestLeft;
      }
      _dir = readButton();
      if (_dir == right) {  //change direction
        Serial.println("Change to right");
        _dir = right;
        status = requestRight;
        lastStatus = requestLeft;
      }
      break;



    case requestRight:
      if (lastStatus != requestRight) Serial.println("Status requestRight");
      storeDirToEEPROM(right);
      url = "/v1/connections?from=" + config.base + "&to=" + config.right  + "&fields[]=connections/from/departure&fields[]=connections/from/prognosis/departure&fields[]=connections/from/departureTimestamp&limit=" + MAX_CONNECTIONS;
      if (lastStatus != requestRight) storeDepartureString(); // if valid url
      if (JSONline.length() > 1) {
        processRequest();

        // exit
        if (lastDepartureTimeStamp != departureTimeStamp && (lastStatus == requestRight || lastStatus == requestRight)) status = idle; // next departure time choosen
        //  Serial.printf("lastDepartureTimeStamp %d departureTimeStamp %d lastStatus %d \n", lastDepartureTimeStamp , departureTimeStamp, lastStatus);
        lastDepartureTimeStamp = departureTimeStamp;
        lastStatus = requestRight;
      }
      _dir = readButton();
      if (_dir == left) {   //change direction
        _dir = left;
        Serial.println("Change to left");
        status = requestLeft;
        lastStatus = requestRight;
      }
      break;


    case recovery:
      Serial.println("------------ Recovery --------------");
      Serial.println("");
      WiFi.mode(WIFI_AP);
      ConfigureWifi();
      ledColor = off;
      Serial.println(currentDirection);

      // exit
      switch (currentDirection) {
        case left:
          status = requestLeft;
          lastStatus = recovery;
          Serial.println("Recovery left");
          break;

        case right:
          status = requestRight;
          lastStatus = recovery;
          Serial.println("Recovery right");
          break;

        default:
          status = idle;
          lastStatus = recovery;
          break;
      }
      cNTP_Update = 999; // trigger NTP immediately
      minTillDep = -999;
      break;

    default:
      break;
  }

  // store NTP time
  if ( cNTP_Update > (config.Update_Time_Via_NTP_Every * 60 )) {
    storeNTPtime();
    if (DateTime.year > 1970) cNTP_Update = 0;  // trigger loop till date is valid
  }

  // store departure time String from openTransport
  if (millis() - waitJSONLoopEntry > loopTime) {
    if (minTillDep > 1 || minTillDep < 0) { // no updates in the last minute
      if (url.length() > 1) storeDepartureString();  // if valid url
      if (JSONline != "") waitJSONLoopEntry = millis();
    }
  }

  // Display LED
  if (millis() - ledCounter > 1000 ) {
    ledCounter = millis();
    ledState = !ledState;
  }

  if (ledState)  led(ledColor);
  else led(off);

  // send Signal (Beep)
  if (freq < 0) setSignal(0, 0); // off
  else setSignal(1, freq);

  if (_lastStatus != status || millis() - waitLoopEntry > 10000) {
    displayStatus();
    waitLoopEntry = millis();
    _lastStatus = status;
  }
  customWatchdog = millis();
}


//------------------------- END LOOP -------------------------------------


void processRequest() {
  long _diffSec, _diffMin;

  int   _positionDeparture = 1;
  do {
    decodeDepartureTime(_positionDeparture);
    if (departureTime != -999) {  // valid time
      _diffSec = departureTime - actualTime;
      if (_diffSec < -10000) _diffSec += 24 * 3600;  // correct if time is before midnight and departure is after midnight
      _diffMin = (_diffSec / 60) - config.wayToStation;
    } else _diffMin = -999;
    _positionDeparture++;
  } while (_diffMin < 0 && _positionDeparture <= MAX_CONNECTIONS + 1);  // next departure if first not reachable

  minTillDep = (_positionDeparture <= MAX_CONNECTIONS) ? _diffMin : -999; // no connection found

  if (minTillDep != -999) {  // valid result
    freq = (minTillDep >= 0 && minTillDep < 10) ? intensity[minTillDep] : freq = -1; //set frequency if minTillDep between 10 and zero minutes
    loopTime = getLoopTime(minTillDep);
  }
}


boolean getStatus() {
  bool stat;
  String _line;

  _line = client.readStringUntil('\n');
  // Serial.print(" statusline ");
  // Serial.println(line);

  int separatorPosition = _line.indexOf("HTTP/1.1");

  // Serial.print(" separatorPosition ");
  // Serial.println(separatorPosition);
  //  Serial.print("Line ");
  // Serial.print(line);
  if (separatorPosition >= 0) {

    if (_line.substring(9, 12) == "200") stat = true;
    else stat = false;
    //   Serial.print("Status ");
    //   Serial.println(stat);
    return  stat;
  }
}


void storeDepartureString() {
  bool ok = false;
  String _line;
  unsigned long serviceTime = millis();

  ledColor = red;

  url.replace(" ", "%20");

  if (!client.connect("transport.opendata.ch", 80)) {
    Serial.println("connection to ... failed");

  } else {
    client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host:" + serverTransport + "\r\n" + "Connection: keep-alive\r\n\r\n");
    // Wait for answer of webservice
    Serial.println(url.substring(1, url.indexOf("&fields")));
    while (!client.available()) {
      // Serial.println("waiting");
    }
    Serial.printf("Client connect. Service time %d \n",  millis() - serviceTime);
    delay(200);
  }
  // Service answered
  ok = getStatus();
  Serial.printf("Got Status. Service time %d \n",  millis() - serviceTime);

  if (ok) {  // JSON packet is avablable
    while (client.available()) {
      yield();
      ledColor = green;
      _line = client.readStringUntil('\n');
      //  Serial.println(_line);

      if (_line.indexOf("connections") > 1) {
        JSONline = _line; // JSON string detected
        Serial.printf("JSONline stored. Service time %d \n", millis() - serviceTime);
      }
    }
  } else  Serial.println("-- No data from Service --");
}






int findJSONkeyword(String keyword0, String keyword1, String keyword2, int pos ) {
  int hi = pos, i;
  String keyword[3];

  keyword[0] = keyword0;
  keyword[1] = keyword1;
  keyword[2] = keyword2;
  i = 0;
  while (keyword[i] != "" && i < 3) {
    hi = JSONline.indexOf(keyword[i], hi + 1);

    i++;
  }
  if (hi > JSONline.length()) hi = 0;
  return hi;
}





void decodeDepartureTime(int pos) {
  int hour;
  int minute;
  int second;
  int i = 0;
  long h1, h2, hh;
  int separatorPosition = 1;
  String keyword[3];

  while (i < pos) {
    separatorPosition = JSONline.indexOf("from", separatorPosition + 1);
    i++;
  }
  // separatorPosition stands at the line requested by calling function
  for (int i = 0; i < 3; i++) keyword[i] = "";
  hh = findJSONkeyword("departure", "", "", separatorPosition);
  h1 = parseJSONDate(hh);

  // Serial.println(JSONline);

  hh = findJSONkeyword("prognosis", "departure", "" , separatorPosition);
  h2 = parseJSONDate(hh);

  hh = findJSONkeyword("departureTimestamp", "", "" , separatorPosition);  // find unique identifier of connection
  departureTimeStamp = parseJSONnumber(hh);
  departureTime = ( h2 > 0) ? h2 : h1;
}


int getTimeStamp(int pos) {

  int hh = findJSONkeyword("departureTimestamp", "", "", pos );
  return JSONline.substring(pos, pos + 4).toInt();
}





long parseJSONDate(int pos) {
  int hi;
  pos = pos + 11;  // adjust for beginning of text
  if (JSONline.substring(pos, pos + 4) != "null" ) {
    pos = pos + 12; // overread date;

    int hour = JSONline.substring(pos, pos + 2).toInt();
    int minute = JSONline.substring(pos + 3, pos + 5).toInt();
    int second = JSONline.substring(pos + 6, pos + 8).toInt();

    // ----------------------- Spieldaten ------------------------------
    // hour = 10;
    //  minute = 28;
    //  second = 0;

    // ----------------------- Spieldaten ------------------------------

    hi = second + 60 * minute + 3600 * hour;
  } else hi = -999;
  return hi;
}



int parseJSONnumber(int pos) {
  pos = pos + 20;
  return JSONline.substring(pos, pos + 10).toInt();
}



defDirection readButton() {
  defDirection dir = none;
  if (!digitalRead(LEFTPIN)) dir = left;
  if (!digitalRead(RIGHTPIN)) dir = right;

  if (dir != none) beep(3);
  return dir;
}


void beep(int _dura) {
  beepOnTime = _dura;
  beepOffTime = 2;
  delay(BEEPTICKER + 10); // wait for next beepTicker
  while (beeperStatus != beeperIdle) yield();
  beepOnTime = 0;

}

void setSignal (int _onTime, int _offTime) {
  if (beeperStatus == beeperIdle) {
    beepOnTime = _onTime;
    beepOffTime = _offTime;
  }
}

// define loop time based on time till departure
int getLoopTime(int _timeTillDeparture) {

  int _loopTime = LOOP_FAST;
  if (_timeTillDeparture > 5) _loopTime = LOOP_SLOW;
  if (_timeTillDeparture == -999) _loopTime = 0;  // no valid info, immediate update required
  return _loopTime;
}

void storeDirToEEPROM(defDirection dir) {

  if (EEPROM.read(300) != dir) {
    Serial.printf("EEPROM direction before %d and after %d \n", EEPROM.read(300), dir);
    Serial.println(dir);
    EEPROM.write(300, dir);
    EEPROM.commit();
  }
}


void printTime(String purpose, long _tim) {

  int hours = _tim / 3600;
  int res = _tim - hours * 3600;
  int minutes = res / 60;
  res = res - (minutes * 60);
  int seconds = res;
  Serial.print(" ");
  Serial.print(purpose);
  Serial.print(" ");
  Serial.print(hours);
  Serial.print(" H ");
  Serial.print(minutes);
  Serial.print(" M ");
  Serial.print(seconds);
}

void displayStatus() {
  printTime("Tim", actualTime);
  printTime("Dep", departureTime);
  Serial.print(" Status ");
  Serial.print(status);
  Serial.print(" lastStatus ");
  Serial.print(lastStatus);
  Serial.print(" minTillDep ");
  Serial.print(minTillDep);
  Serial.print(" loopTime ");
  Serial.print(loopTime);
  Serial.print(" freq ");
  Serial.println(freq);
}


void ISRbeepTicker(void *pArg) {

  switch (beeperStatus) {
    case beeperIdle:
      beepOnTimer  = beepOnTime;
      beepOffTimer = beepOffTime;

      // exit
      if (beepOnTime > 0) beeperStatus = beeperOn;
      break;

    case beeperOff:
      digitalWrite(BEEPPIN, LOW); // always off
      beepOffTimer--;
      // exit
      if (beepOffTimer <= 0) {
        beeperStatus = beeperIdle;
      }
      break;

    case beeperOn:
      if (beepOffTimer > 0) beepOnTimer--;
      digitalWrite(BEEPPIN, HIGH);

      // exit
      if (beepOnTimer <= 0) {
        beeperStatus = beeperOff;
      }
      break;

    default:
      break;
  }
}



//------------------- OTA ---------------------------------------
void otaInit() {

  led(red);

  for (int i = 0; i < 3; i++) beep(3);
  WiFi.mode(WIFI_AP);
  ConfigureWifi();
  MDNS.begin(host);
  MDNS.addService("arduino", "tcp", aport);
  OTA.begin(aport);
  TelnetServer.begin();
  TelnetServer.setNoDelay(true);
  Serial.print("IP address: ");
  led(green);
  Serial.println(WiFi.localIP());
  Serial.println("OTA settings applied");
}

void otaReceive() {
  if (OTA.parsePacket()) {
    IPAddress remote = OTA.remoteIP();
    int cmd  = OTA.parseInt();
    int port = OTA.parseInt();
    int size   = OTA.parseInt();

    Serial.print("Update Start: ip:");
    Serial.print(remote);
    Serial.printf(", port:%d, size:%d\n", port, size);
    uint32_t startTime = millis();

    WiFiUDP::stopAll();

    if (!Update.begin(size)) {
      Serial.println("Update Begin Error");
      return;
    }

    WiFiClient client;
    if (client.connect(remote, port)) {

      uint32_t written;
      while (!Update.isFinished()) {
        written = Update.write(client);
        if (written > 0) client.print(written, DEC);
      }
      Serial.setDebugOutput(false);

      if (Update.end()) {
        client.println("OK");
        Serial.printf("Update Success: %u\nRebooting...\n", millis() - startTime);
        ESP.restart();
      } else {
        Update.printError(client);
        Update.printError(Serial);
      }
    } else {
      Serial.printf("Connect Failed: %u\n", millis() - startTime);
    }
  }
  //IDE Monitor (connected to Serial)
  if (TelnetServer.hasClient()) {
    if (!Telnet || !Telnet.connected()) {
      if (Telnet) Telnet.stop();
      Telnet = TelnetServer.available();
    } else {
      WiFiClient toKill = TelnetServer.available();
      toKill.stop();
    }
  }
  if (Telnet && Telnet.connected() && Telnet.available()) {
    while (Telnet.available())
      Serial.write(Telnet.read());
  }
  if (Serial.available()) {
    size_t len = Serial.available();
    uint8_t * sbuf = (uint8_t *)malloc(len);
    Serial.readBytes(sbuf, len);
    if (Telnet && Telnet.connected()) {
      Telnet.write((uint8_t *)sbuf, len);
      yield();
    }
    free(sbuf);
  }
}
