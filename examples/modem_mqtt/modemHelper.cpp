#include <SoftwareSerial.h>
#include "sim800.h"

#define halt(s) { Serial.println(F( s )); while(1);  }

extern Modem modem;
extern SoftwareSerial ss;

boolean modemSetup(const __FlashStringHelper *apn) {
  //Watchdog.reset();

  Serial.println(F("Initializing modem....(May take 3 seconds)"));
  
  ss.begin(9600); // if you're using software serial
  if (! modem.begin(ss)) {          
    Serial.println(F("Couldn't find modem"));
    return false;
  }
  ss.println("AT+CMEE=2");
  Serial.println(F("Modem is OK"));
  //Watchdog.reset();
  Serial.println(F("Checking for network..."));
  while (modem.getNetworkStatus() != 1) {
   delay(500);
  }
  modem.setGPRSNetworkSettings(apn);

  Serial.println(F("Disabling GPRS"));
  modem.enableGPRS(false);
  
  //Watchdog.reset();
  delay(5000);  // wait a few seconds to stabilize connection
  //Watchdog.reset();

  Serial.println(F("Enabling GPRS"));
  if (!modem.enableGPRS(true)) {
    Serial.println(F("Failed to turn GPRS on"));  
    return false;
  }
  //Watchdog.reset();

  return true;
}
