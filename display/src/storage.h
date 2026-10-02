#ifndef _AB_SDCARD
#define _AB_SDCARD

#include <SdFat.h>
#include <ArduinoJson.h>

#define SD_CS 15

class Storage {
 public:
  SdFat fs;

  void setup();

  File32 open(const char* path, oflag_t flags);
  bool loadJson(const char* path, JsonDocument& json);
};

#endif
