// The MIT License (MIT)
//
// Copyright (c) 2015 Adafruit Industries
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#ifndef _SIM800_MQTT_H_
#define _SIM800_MQTT_H_

#include "sim800.h"
#include "Adafruit_MQTT.h"

#define MQTT_FONA_INTERAVAILDELAY 100
#define MQTT_FONA_QUERYDELAY 500


// FONA-specific version of the Adafruit_MQTT class.
// Note that this is defined as a header-only class to prevent issues with using
// the library on non-FONA platforms (since Arduino will include all .cpp files
// in the compilation of the library).
class Modem_MQTT : public Adafruit_MQTT {
 public:
  Modem_MQTT(Modem *f, const char *server, uint16_t port,
                       const char *cid, const char *user, const char *pass):
    Adafruit_MQTT(server, port, cid, user, pass),
    dev(f)
  {}

  Modem_MQTT(Modem *f, const char *server, uint16_t port,
                     const char *user="", const char *pass=""):
    Adafruit_MQTT(server, port, user, pass),
    dev(f)
  {}

  bool connectServer() {
    char server[40];
    strncpy(server, servername, 40);
#ifdef ADAFRUIT_SLEEPYDOG_H
    Watchdog.reset();
#endif

    // connect to server
    DEBUG_PRINTLN(F("Connecting to TCP"));
    return dev->TCPconnect(server, portnum);
  }

  bool disconnectServer() {
    return dev->TCPclose();
  }

  bool connected() {
    // Return true if connected, false if not connected.
    return dev->TCPconnected();
  }

  uint16_t readPacket(uint8_t *buffer, uint16_t maxlen, int16_t timeout) {
    uint8_t *buffp = buffer;
    DEBUG_PRINTLN(F("Reading data.."));

    if (!dev->TCPconnected()) return 0;


    /* Read data until either the connection is closed, or the idle timeout is reached. */
    uint16_t len = 0;
    int16_t t = timeout;
    uint16_t avail;

    uint32_t lastRead = millis();
    while (dev->TCPconnected() && (millis() - lastRead < timeout)) {
      //DEBUG_PRINT('.');
      while (avail = dev->TCPavailable()) {
        //DEBUG_PRINT('!');

        if (len + avail > maxlen) {
	  avail = maxlen - len;
	  if (avail == 0) return len;
        }

        // try to read the data into the end of the pointer
        if (! dev->TCPread(buffp, avail)) return len;

        // read it! advance pointer
        buffp += avail;
        len += avail;
        lastRead = millis();
        //timeout = t;  // reset the timeout

        //DEBUG_PRINTLN((uint8_t)c, HEX);

        if (len == maxlen) {  // we read all we want, bail
          DEBUG_PRINT(F("Read:\t"));
          DEBUG_PRINTBUFFER(buffer, len);
	  return len;
        }
      }
#ifdef ADAFRUIT_SLEEPYDOG_H
      Watchdog.reset();
#endif
      /*timeout -= MQTT_FONA_INTERAVAILDELAY;
      timeout -= MQTT_FONA_QUERYDELAY; // this is how long it takes to query the FONA for avail()
      delay(MQTT_FONA_INTERAVAILDELAY);*/
    }
    
    return len;
  }

  bool sendPacket(uint8_t *buffer, uint16_t len) {
    DEBUG_PRINTLN(F("Writing packet"));
    if (dev->TCPconnected()) {
      boolean ret = dev->TCPsend((char *)buffer, len);
      //DEBUG_PRINT(F("sendPacket returned: ")); DEBUG_PRINTLN(ret);
      if (!ret) {
        DEBUG_PRINTLN("Failed to send packet.");
        return false;
      }
    } else {
      DEBUG_PRINTLN(F("Connection failed!"));
      return false;
    }
    return true;
  }

 private:
  uint32_t serverip;
  Modem *dev;
};


#endif
