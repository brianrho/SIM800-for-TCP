/***************************************************
  This is a library for our Adafruit FONA Cellular Module

  Designed specifically to work with the Adafruit FONA
  ----> http://www.adafruit.com/products/1946
  ----> http://www.adafruit.com/products/1963

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
  
  Modified by Brian Ejike (2017)
 ****************************************************/

    
#ifndef _SIM800_H
#define _SIM800_H

#include "Arduino.h"
#include <avr/pgmspace.h>

#define FONA_DEFAULT_TIMEOUT_MS 500



//#define MODEM_DEBUG

#ifdef MODEM_DEBUG
    #define DEBUG_PRINT(...)		DebugStream.print(__VA_ARGS__)
    #define DEBUG_PRINTLN(...)		DebugStream.println(__VA_ARGS__)
#else 
    #define DEBUG_PRINT(...)
    #define DEBUG_PRINTLN(...)
#endif

// DebugStream	sets the Stream output to use
// for debug (only applies when ADAFRUIT_FONA_DEBUG
// is defined in config)
#define DebugStream		Serial

#define prog_char  					char PROGMEM

#define prog_char_strcmp(a, b)					strcmp_P((a), (b))
// define prog_char_strncmp(a, b, c)				strncmp_P((a), (b), (c))
#define prog_char_strstr(a, b)					strstr_P((a), (b))
#define prog_char_strlen(a)						strlen_P((a))
#define prog_char_strcpy(to, fromprogmem)		strcpy_P((to), (fromprogmem))
//define prog_char_strncpy(to, from, len)		strncpy_P((to), (fromprogmem), (len))


class Modem {
 public:
  Modem(int8_t r);
  boolean begin(Stream &port);

  // FONA 3G requirements
  boolean setBaudrate(uint32_t baud);

  // RTC
  boolean enableRTC(uint8_t i);
  boolean readRTC(uint8_t *year, uint8_t *month, uint8_t *date, uint8_t *hr, uint8_t *min, uint8_t *sec);

  // Battery and ADC
  boolean getADCVoltage(uint16_t *v);
  boolean getBattPercent(uint16_t *p);
  boolean getBattVoltage(uint16_t *v);

  // SIM query
  uint8_t getNetworkStatus(void);
  uint8_t getRSSI(void);

  // IMEI
  uint8_t getIMEI(char *imei);

  boolean sendUSSD(char *ussdmsg, char *ussdbuff, uint16_t maxlen, uint16_t *readlen);

  // Time
  boolean enableNetworkTimeSync(boolean onoff);
  boolean getTime(char *buff, uint16_t maxlen);

  // GPRS handling
  boolean enableGPRS(boolean onoff);
  uint8_t GPRSstate(void);
  void setGPRSNetworkSettings(const __FlashStringHelper* apn, const __FlashStringHelper* username=0, const __FlashStringHelper* password=0);

  // TCP raw connections
  boolean TCPconnect(char *server, uint16_t port);
  boolean TCPclose(void);
  boolean TCPconnected(void);
  boolean TCPsend(char *packet, uint8_t len);
  uint16_t TCPavailable(void);
  uint16_t TCPread(uint8_t *buff, uint8_t len);

  // Helper functions to verify responses.
  boolean expectReply(const __FlashStringHelper* reply, uint16_t timeout = 10000);
  boolean sendCheckReply(char *send, char *reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReply(const __FlashStringHelper* send, const __FlashStringHelper* reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReply(char* send, const __FlashStringHelper* reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);


 protected:
  int8_t _rstpin;
  uint8_t _type;

  char replybuffer[128];
  const __FlashStringHelper* apn;
  const __FlashStringHelper* apnusername;
  const __FlashStringHelper* apnpassword;
  const __FlashStringHelper* useragent;
  const __FlashStringHelper* ok_reply;

  // HTTP helpers
  boolean HTTP_setup(char *url);

  void flushInput();
  uint16_t readRaw(uint16_t b);
  uint8_t readline(uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS, boolean multiline = false);
  uint8_t getReply(char *send, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  uint8_t getReply(const __FlashStringHelper* send, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  uint8_t getReply(const __FlashStringHelper* prefix, char *suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  uint8_t getReply(const __FlashStringHelper* prefix, int32_t suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  uint8_t getReply(const __FlashStringHelper* prefix, int32_t suffix1, int32_t suffix2, uint16_t timeout); // Don't set default value or else function call is ambiguous.
  uint8_t getReplyQuoted(const __FlashStringHelper* prefix, const __FlashStringHelper* suffix, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);

  boolean sendCheckReply(const __FlashStringHelper* prefix, char *suffix, const __FlashStringHelper* reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReply(const __FlashStringHelper* prefix, int32_t suffix, const __FlashStringHelper* reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReply(const __FlashStringHelper* prefix, int32_t suffix, int32_t suffix2, const __FlashStringHelper* reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);
  boolean sendCheckReplyQuoted(const __FlashStringHelper* prefix, const __FlashStringHelper* suffix, const __FlashStringHelper* reply, uint16_t timeout = FONA_DEFAULT_TIMEOUT_MS);


  boolean parseReply(const __FlashStringHelper* toreply,
          uint16_t *v, char divider  = ',', uint8_t index=0);
  boolean parseReply(const __FlashStringHelper* toreply,
          char *v, char divider  = ',', uint8_t index=0);
  boolean parseReplyQuoted(const __FlashStringHelper* toreply,
          char *v, int maxlen, char divider, uint8_t index);

  boolean sendParseReply(const __FlashStringHelper* tosend,
       const __FlashStringHelper* toreply,
       uint16_t *v, char divider = ',', uint8_t index=0);

  Stream *mySerial;
};

#endif