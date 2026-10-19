#pragma once

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>
#include <Wire.h>
#include <stdint.h>

/* ========================================================================= */
/*                                                          global constants */
/* ========================================================================= */

#define SERIAL_BAUD 115200

#define SOUND_MODULE_ID 0x10
#define DISPLAY_MODULE_ID 0x11
#define NUMPAD_MODULE_ID 0x20

#ifdef ARDUINO_ARCH_ESP32
const int WIRE_SDA = 21;
const int WIRE_SCL = 22;
const uint8_t LED_BUILTIN = 14;
#else
const int WIRE_SDA = A4;
const int WIRE_SCL = A5;
#endif

#define ID_BUFFER_SIZE 26
#define PARAM_BUFFER_SIZE 10
#define PRINT_BUFFER_SIZE 128
#define REQUEST_BUFFER_SIZE 32
#define PATH_BUFFER_SIZE 64

#define DEFAULT_BITMAP_WIDTH 320
#define DEFAULT_BITMAP_HEIGHT 240

#define DEFAULT_BUZZER 12
#define DEFAULT_BUZZER_FREQUENCY 150
#define DEFAULT_BUZZER_TIMEOUT 250

/* ========================================================================= */
/*                                                     global activity board */
/* ========================================================================= */

using ModuleId = uint8_t;

class ActivityBoard {
 public:
  void setup();
  void setup(ModuleId moduleId);
  void clearI2CBus(uint8_t sda = WIRE_SDA, uint8_t scl = WIRE_SCL);
};
extern ActivityBoard activityBoard;

/* ========================================================================= */
/*                                                                logger api */
/* ========================================================================= */

class ActivityLogger {
 public:
  void serialPrintln(const char* fmt, ...);
#if defined(ARDUINO_ARCH_ESP32)
  void serialPrintln(const char* fmt, bool isFlash, va_list args);
#else
  void serialPrintln(const __FlashStringHelper* fmt, ...);
  void serialPrintln(const char* fmt, bool isFlash, va_list args);
#endif
  void serialDump(const unsigned char* buffer, uint8_t length);
};
extern ActivityLogger activityLogger;

/* Logger-Makro-Weiche für ESP32 und Nano */

#if defined(ARDUINO_ARCH_ESP32)
#define LOG(fmt, ...) activityLogger.serialPrintln(fmt, ##__VA_ARGS__)
#else
#define LOG(fmt, ...) activityLogger.serialPrintln(F(fmt), ##__VA_ARGS__)
#endif

#define DUMP(buffer, length) activityLogger.serialDump((const unsigned char*)buffer, length)

/* ========================================================================= */
/*                                                          controls helpers */
/* ========================================================================= */

class ActivityControls {
 public:
  void blink(uint8_t pin, uint16_t cycleCount, uint16_t timeout);
  void blink(const uint8_t pins[], uint8_t pinCount, uint16_t cycleCount, uint16_t timeout);
  bool buttonPressed(uint8_t pin);
  void buzzer(uint8_t buzzer = DEFAULT_BUZZER, int frequency = DEFAULT_BUZZER_FREQUENCY, long timeout = DEFAULT_BUZZER_TIMEOUT);
  void scanWire();
};
extern ActivityControls activityControls;
