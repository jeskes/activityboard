#ifndef _AB_TEXT
#define _AB_TEXT

#include <ArduinoJson.h>
#include <ablib.h>

#include <map>

#include "storage.h"

class ClientContext {
 public:
  ClientContext(Storage& storage);

  void setup();
  void init(uint16_t clientId);

  void clearParams();
  void clearParam(const char* name);
  void putParam(const char* name, const char* value, byte append);

  String formatMessage(const char* id);
  const char* getMessage(const char* id);

  String getBitmapPath(const char* id);

 private:
  Storage& storage;
  uint16_t clientId;

  std::map<String, String> params;
  void initParams(uint16_t clientId);

  JsonDocument messages;
  void initMessages(uint16_t clientId);

  JsonDocument bitmaps;
  void initBitmaps(uint16_t clientId);

  String utf8ToExtendedAscii(const String& utf8);
};

#endif
