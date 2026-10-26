#pragma once

#ifdef ARDUINO_ARCH_ESP32

#include <ArduinoJson.h>
#include <FS.h>
#include <SD.h>

#define DEFAULT_SD_CLK 18
#define DEFAULT_SD_MISO 19
#define DEFAULT_SD_MOSI 23
#define DEFAULT_SD_CS 5

#define USE_DEFAULT_PIN 255

class Storage {
 public:
  Storage();

  fs::SDFS& sd;

  void setup(uint8_t clk = USE_DEFAULT_PIN, uint8_t miso = USE_DEFAULT_PIN, uint8_t mosi = USE_DEFAULT_PIN, uint8_t cs = USE_DEFAULT_PIN);
  void diagnostics();

  File open(const char* path);
  bool loadJson(const char* path, JsonDocument& json);

 private:
  void list(const char* folder = "/");
};

#endif