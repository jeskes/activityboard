#pragma once

#ifdef ARDUINO_ARCH_ESP32

#include <ArduinoJson.h>
#include <FS.h>
#include <SD.h>

#include <type_traits>

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

  template <typename F>
  auto sdAccess(F&& func) -> decltype(func()) {
    if (xSemaphoreTake(sdMutex, portMAX_DELAY) == pdTRUE) {
      if constexpr (std::is_void_v<decltype(func())>) {
        func();
        xSemaphoreGive(sdMutex);
      }
      else {
        auto result = func();
        xSemaphoreGive(sdMutex);
        return result;
      }
    }
    return decltype(func())(); /* fallback */
  }

 private:
  SemaphoreHandle_t sdMutex;
  void list(const char* folder = "/");
};

#endif