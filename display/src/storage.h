#ifndef _AB_SDCARD
#define _AB_SDCARD

#include <ArduinoJson.h>
#include <FS.h>
#include <SD.h>
#include <ablib.h>

class Storage {
 public:
  Storage();

  fs::SDFS& sd;

  void setup();
  void diagnostics();

  File open(const char* path);
  bool loadJson(const char* path, JsonDocument& json);

 private:
  void list(const char* folder = "/");
};

#endif
