#pragma once

#ifdef ARDUINO_ARCH_ESP32

#include <ArduinoJson.h>
#include <FS.h>
#include <SD.h>

#include <type_traits>

inline constexpr uint8_t DEFAULT_SD_CLK = 18;
inline constexpr uint8_t DEFAULT_SD_MISO = 19;
inline constexpr uint8_t DEFAULT_SD_MOSI = 23;
inline constexpr uint8_t DEFAULT_SD_CS = 5;
inline constexpr uint8_t DEFAULT_SD_PIN = 0xFF;

class Storage {
 public:
  Storage();

  fs::SDFS& sd;

  void setup(uint8_t clk = DEFAULT_SD_PIN, uint8_t miso = DEFAULT_SD_PIN, uint8_t mosi = DEFAULT_SD_PIN, uint8_t cs = DEFAULT_SD_PIN);
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