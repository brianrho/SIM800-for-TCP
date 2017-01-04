#include <SoftwareSerial.h>
#include <sim800.h>

#define FONA_RST 4  // no need to connect this; 

SoftwareSerial modemSerial = SoftwareSerial(6,7);

Modem modem = Modem(FONA_RST);

void setup() {

  while (! Serial);
  
  Serial.begin(9600);
  Serial.println(F("SIM800 TCP/HTTP demo"));
  Serial.println(F("Initializing modem... (May take a few seconds)"));

  modemSerial.begin(9600);
  if (! modem.begin(modemSerial)) {
    Serial.println(F("Couldn't find modem"));
    while(1);
  }
  Serial.print(F("Checking for Cell network..."));
  while (modem.getNetworkStatus() != 1);
  
  Serial.println(F("Registered."));
  modem.setGPRSNetworkSettings(F("web.gprs.mtnnigeria.net"));
  modem.enableGPRS(true);
  if (modem.TCPconnect("httpbin.org", 80)){
    char req[] = "GET /ip HTTP/1.1\r\nHost: httpbin.org\r\n\r\n";
    modem.TCPsend(req, strlen(req));
    uint8_t recv[21];
    unsigned long lastRead = millis();
    while (millis() - lastRead < 2000){
      while (modem.TCPavailable()){
        uint8_t r = modem.TCPread(recv, 20);
        recv[r] = 0;
        Serial.write((char *)recv);
        lastRead = millis();
      }
    }
    modem.TCPclose();
    modem.enableGPRS(false);
  }
}

void loop() {
  
}
