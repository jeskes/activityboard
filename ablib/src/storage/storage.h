#pragma once

#ifdef ARDUINO_ARCH_ESP32

#include <ArduinoJson.h>
#include <FS.h>
#include <SD.h>

#define SD_SCK 18
#define SD_MISO 19
#define SD_MOSI 23
#define SD_CS 5

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