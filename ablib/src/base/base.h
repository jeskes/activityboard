#pragma once

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>
#include <Wire.h>
#include <stdint.h>

/* ========================================================================= */
/*                                                          global constants */
/* ========================================================================= */

#define SERIAL_BAUD 115200

#define CORE_MODULE_ID 0x10
#define NUMPAD_MODULE_ID 0x20

#ifdef ARDUINO_ARCH_ESP32
inline constexpr int WIRE_SDA = 21;
inline constexpr int WIRE_SCL = 22;
inline constexpr uint8_t LED_BUILTIN = 14;
#else
#define IRAM_ATTR /* blank */
inline constexpr uint8_t WIRE_SDA = A4;
inline constexpr uint8_t WIRE_SCL = A5;
#endif

inline constexpr uint8_t ID_BUFFER_SIZE = 26;
inline constexpr uint8_t PARAM_NAME_BUFFER_SIZE = 10;
inline constexpr uint8_t PARAM_VALUE_BUFFER_SIZE = 16;
inline constexpr uint8_t PRINT_BUFFER_SIZE = 128;
inline constexpr uint8_t PRINT_LINE_SIZE = 3;
inline constexpr uint8_t PRINT_FILE_SIZE = 20;
inline constexpr uint8_t REQUEST_BUFFER_SIZE = 32;
inline constexpr uint8_t PATH_BUFFER_SIZE = 64;
inline constexpr uint8_t REQUEST_QUEUE_SIZE = 16;

inline constexpr uint16_t DEFAULT_IMAGE_WIDTH = 320;
inline constexpr uint16_t DEFAULT_IMAGE_HEIGHT = 240;

inline constexpr uint8_t DEFAULT_BUZZER = 12;
inline constexpr int DEFAULT_BUZZER_FREQUENCY = 150;
inline constexpr unsigned long DEFAULT_BUZZER_TIMEOUT = 250;

constexpr unsigned long BUTTON_PRESSED_TIMEOUT = 200;

constexpr unsigned long CORECLIENT_HEARTBEAT_TIMEOUT = 2000;

/* ========================================================================= */
/*                                                             general types */
/* ========================================================================= */

using ModuleId = uint8_t;

using ClientId = uint8_t;
inline constexpr ClientId SYSTEM_CLIENT_ID = 10;

using ParameterId = const char*;
using TrackId = uint8_t;
using TrackFolder = uint8_t;
using MusicId = const char*;
using MessageId = const char*;
using ImageId = const char*;
using MenuId = const char*;
using DataId = const char*;

using Input = uint8_t;
/* predefined input values */

namespace StdInput {
inline constexpr Input NONE = 0xff;
inline constexpr Input NUM_0 = 0x00;
inline constexpr Input NUM_1 = 0x01;
inline constexpr Input NUM_2 = 0x02;
inline constexpr Input NUM_3 = 0x03;
inline constexpr Input NUM_4 = 0x04;
inline constexpr Input NUM_5 = 0x05;
inline constexpr Input NUM_6 = 0x06;
inline constexpr Input NUM_7 = 0x07;
inline constexpr Input NUM_8 = 0x08;
inline constexpr Input NUM_9 = 0x09;
inline constexpr Input ASTERISK = 0x0A;
inline constexpr Input HASH = 0x0B;
inline constexpr Input START = 0x10;
inline constexpr Input STOP = 0x11;
inline constexpr Input NEXT = 0x12;
inline constexpr Input PREV = 0x13;
inline constexpr Input REPEAT = 0x14;
inline constexpr Input PAUSE = 0x15;
inline constexpr Input RESUME = 0x16;
inline constexpr Input CUSTOM = 0x20; /* start ax 0x20 */
};  // namespace StdInput

/* ========================================================================= */
/*                                                     global activity board */
/* ========================================================================= */

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
  void print(const char* fmt, ...);
  void println(const char* fmt, ...);
#if defined(ARDUINO_ARCH_ESP32)
  void print(const char* fmt, bool isFlash, va_list args);
  void println(const char* fmt, bool isFlash, va_list args);
#else
  void print(const __FlashStringHelper* fmt, ...);
  void print(const char* fmt, bool isFlash, va_list args);
  void println(const __FlashStringHelper* fmt, ...);
  void println(const char* fmt, bool isFlash, va_list args);
#endif
  void printLocation(const char* file, unsigned int line);
  void dump(const unsigned char* bytes, uint8_t length);
  void tohex(const unsigned char* bytes, uint8_t length, char* buffer);
};

extern ActivityLogger activityLogger;

#ifdef DEBUG
#if defined(ARDUINO_ARCH_ESP32)
#define LOG(fmt, ...)                                 \
  {                                                   \
    activityLogger.printLocation(__FILE__, __LINE__); \
    activityLogger.println(fmt, ##__VA_ARGS__);       \
  }
#else
#define LOG(fmt, ...)                                 \
  {                                                   \
    activityLogger.printLocation(__FILE__, __LINE__); \
    activityLogger.println(F(fmt), ##__VA_ARGS__);    \
  }
#endif
#define DUMP(buffer, length)                                   \
  {                                                            \
    activityLogger.printLocation(__FILE__, __LINE__);          \
    activityLogger.dump((const unsigned char*)buffer, length); \
  }
#else
#define LOG(fmt, ...)
#define DUMP(buffer, length)
#endif

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
