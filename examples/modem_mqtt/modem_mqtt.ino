#include <SoftwareSerial.h>
#include "sim800_mqtt.h"
#include "Adafruit_MQTT.h"

SoftwareSerial ss = SoftwareSerial(6, 7);

Modem modem = Modem(4);  //RST

  // Optionally configure a GPRS APN, username, and password.
  // You might need to do this to access your network's GPRS/data
  // network.  Contact your provider for the exact APN, username,
  // and password values.  Username and password are optional and
  // can be removed, but APN is required.
#define APN       "web.gprs.mtnnigeria.net"

#define SERVER      "cloud.nodewire.org"
#define SERVERPORT  1883

// Setup the SIM800 MQTT class by passing in the Modem object and MQTT server and login details.
Modem_MQTT mqtt(&modem, SERVER, SERVERPORT, "arduinoClient");

// You don't need to change anything below this line!
#define halt(s) { Serial.println(F( s )); while(1);  }

// modemSetup is a helper function that sets up the SIM800 and connects to
// the GPRS network. See the modemHelper.cpp tab above for the source!
boolean modemSetup(const __FlashStringHelper *apn);

/****************************** Feeds ***************************************/

Adafruit_MQTT_Subscribe iot_in = Adafruit_MQTT_Subscribe(&mqtt, "sadiqahmad2/#");

/*************************** Sketch Code ************************************/

void setup() {
  while (!Serial);

  // Watchdog is optional!
  //Watchdog.enable(8000);
  
  Serial.begin(9600);

  Serial.println(F("SIM800 MQTT demo"));  
  
  // Initialise the modem
  while (! modemSetup(F(APN))) {
    Serial.println("Retrying modem");
  }

  Serial.println(F("Connected to Cellular!"));

  mqtt.subscribe(&iot_in);
  Serial.println("Subscribed to sadiqahmad/#");
  
  //Watchdog.reset();
  delay(5000);  // wait a few seconds to stabilize connection
  //Watchdog.reset();
}

void loop() {
  // Make sure to reset watchdog every loop iteration!
  //Watchdog.reset();

  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  //Watchdog.reset();  
  // this is our 'wait for incoming subscription packets' busy subloop
  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &iot_in) {
      Serial.print(F("\tReceived: "));
      Serial.println((char *)iot_in.lastread);
    }
  }

  //ping the server to keep the mqtt connection alive, only needed if we're not publishing
  if(! mqtt.ping()) {
    Serial.println(F("MQTT Ping failed."));
  }

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
  }
  Serial.println("MQTT Connected!");
}
