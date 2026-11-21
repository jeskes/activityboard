#pragma once

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>
#include <Wire.h>
#include <stdint.h>

/* ========================================================================= */
/*                                                          global constants */
/* ========================================================================= */

#define SERIAL_BAUD 115200

inline constexpr uint8_t CORE_MODULE_ID = 0x10;
inline constexpr uint8_t NUMPAD_MODULE_ID = 0x20;
inline constexpr uint8_t GADGET_EXPANSION_ID1 = 0x26;
inline constexpr uint8_t GADGET_EXPANSION_ID2 = 0x27;

#ifdef ARDUINO_ARCH_ESP32
inline constexpr uint8_t WIRE_SDA = 21;
inline constexpr uint8_t WIRE_SCL = 22;
#ifndef LED_BUILTIN
inline constexpr uint8_t LED_BUILTIN = 14;
#endif
#else
inline constexpr uint8_t WIRE_SDA = A4;
inline constexpr uint8_t WIRE_SCL = A5;
#define IRAM_ATTR /* blank */
#endif
inline constexpr uint32_t WIRE_SPEED = 100000;

inline constexpr uint8_t ID_BUFFER_SIZE = 28;
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

inline constexpr unsigned long DEBOUNCE_DELAY = 250;

inline constexpr unsigned long CORECLIENT_HEARTBEAT_TIMEOUT = 2000;
inline constexpr unsigned long CORECLIENT_DISCONNECTED_TIMEOUT = 5000;

/* ========================================================================= */
/*                                                             general types */
/* ========================================================================= */

using ModuleId = uint8_t;

using ClientId = uint8_t;
inline constexpr ClientId SYSTEM_CLIENT_ID = 10;

using ParameterId = const char*;
using TrackPath = const char*;
using MessageId = const char*;
using ImageId = const char*;
using MenuId = const char*;
using DataId = const char*;
using RfidId = const char*;
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

/* typed constants */

/* flags base class */

template <typename Derived>
struct __attribute__((packed)) CoreFlags {
  CoreFlags() {
  }

  CoreFlags(const uint8_t& flags) {
    this->flags = flags;
  }

  explicit operator uint8_t() const {
    return flags;
  }

  Derived& operator=(uint8_t byte) {
    flags = byte;
    return static_cast<Derived&>(*this);
  }

  Derived& clear() {
    return operator=(0);
  }

  Derived& operator+=(uint8_t flag) {
    flags |= flag;
    return static_cast<Derived&>(*this);
  }

  Derived& operator-=(uint8_t flag) {
    flags &= ~flag;
    return static_cast<Derived&>(*this);
  }

  bool has(uint8_t flag) const {
    return (flags & flag) != 0;
  }

  uint8_t flags = 0;
};

/* ========================================================================= */
/*                                                             wifi settings */
/* ========================================================================= */

inline constexpr unsigned long WIFI_ACTIVATION_POLLING = 20;
inline constexpr unsigned long WIFI_ACTIVE_POLLING = 5;
inline constexpr const char* WIFI_SSID = "fizzbizz";
inline constexpr const char* WIFI_PASSPHRASE = "fizzbizz";
inline constexpr const char* WIFI_USERNAME = "fizzbizz";
inline constexpr const char* WIFI_PASSWORD = "fizzbizz";
inline constexpr const uint32_t WIFI_TIMEOUT = 10000;

/* ========================================================================= */
/*                                                               maintenance */
/* ========================================================================= */

inline constexpr const char* MAINTENANCE_SSID = "tecsi";
inline constexpr const char* MAINTENANCE_PASSPHRASE = "wiseguys";
inline constexpr const char* MAINTENANCE_FTP_USERNAME = "fizzbizz";
inline constexpr const char* MAINTENANCE_FTP_PASSWORD = "fizzbizz";
inline constexpr const char* MAINTENANCE_OTA_PASSWORD = "";
inline constexpr const char* MAINTENANCE_HOST_CORE = "core";
inline constexpr const char* MAINTENANCE_HOST_SOUND = "sound";

/* ========================================================================= */
/*                                                     global activity board */
/* ========================================================================= */

class ActivityBoard {
 public:
  void setup();
  void setup(ModuleId moduleId);
  void clearI2CBus(uint8_t sda, uint8_t scl);
};

extern ActivityBoard activityBoard;

/* ========================================================================= */
/*                                                                logger api */
/* ========================================================================= */

class ActivityLogger {
 public:
  void setup();
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

#if defined(DEBUG) || defined(AB_DEBUG)
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
