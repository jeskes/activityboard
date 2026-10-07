#pragma once

#include <ArduinoJson.h>
#include <FS.h>
#include <SD.h>
#include <ablib.h>

class Storage {
 public:
  Storage();

  void setup();
  void diagnostics();

  fs::SDFS& sd;

  File open(const char* path);
  bool loadJson(const char* path, JsonDocument& json);

 private:
  void list(const char* folder = "/");
};
