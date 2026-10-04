#ifndef _AB_LIB
#define _AB_LIB

#include <Arduino.h>
#include <I2CKeyPad.h>
#include <TaskSchedulerDeclarations.h>
#include <stdint.h>  // Zwingend erforderlich für plattformunabhängige Datentypen

/* constants */

#define SERIAL_BAUD 115200

#define SOUND_MODULE_ID 0x10
#define DISPLAY_MODULE_ID 0x11
#define NUMPAD_MODULE_ID 0x20

#ifdef ARDUINO_ARCH_ESP32
const int WIRE_SDA = 21;
const int WIRE_SCL = 22;
const uint8_t LED_BUILTIN = 14;
#endif

#define ID_BUFFER_SIZE 16
#define PRINT_BUFFER_SIZE 128
#define REQUEST_BUFFER_SIZE 32

/* common operations */

class ActivityBoard {
 public:
  void setup();
  void setup(uint8_t moduleId);
};
extern ActivityBoard activityBoard;

class ActivityLogger {
 public:
  void serialPrintln(const char* fmt, ...);
#if defined(ARDUINO_ARCH_ESP32)
  void serialPrintln(const char* fmt, bool isFlash, va_list args);
#else
  void serialPrintln(const __FlashStringHelper* fmt, ...);
  void serialPrintln(const char* fmt, bool isFlash, va_list args);
#endif
};
extern ActivityLogger activityLogger;

/* Logger-Makro-Weiche für ESP32 und Nano */

#if defined(ARDUINO_ARCH_ESP32)
#define LOG(fmt, ...) activityLogger.serialPrintln(fmt, ##__VA_ARGS__)
#else
#define LOG(fmt, ...) activityLogger.serialPrintln(F(fmt), ##__VA_ARGS__)
#endif

class ActivityControls {
 public:
  void blink(uint8_t pin, uint16_t cycleCount, uint16_t timeout);
  void blink(const uint8_t pins[], uint8_t pinCount, uint16_t cycleCount, uint16_t timeout);
  bool buttonPressed(uint8_t pin);
};
extern ActivityControls activityControls;

/* peripherical api */

enum class RequestType : uint16_t {
  UNKNOWN = 0,
  SOUND_INIT = 0x1000,
  SOUND_PLAY = 0x1001,
  SOUND_STOP = 0x1002,
  DISPLAY_INIT = 0x2000,
  DISPLAY_CLEAR = 0x2001,
  DISPLAY_PARAMETER = 0x2002,
  DISPLAY_MESSAGE = 0x2003,
  DISPLAY_BITMAP = 0x2004,
};

enum class StatusType : uint16_t {
  UNKNOWN = 0,
  SOUND_STATUS = 0x1001,
  DISPLAY_STATUS = 0x2001,
};

struct __attribute__((packed)) BaseRequest {
  RequestType type;
  BaseRequest(RequestType t)
      : type(t) {
  }
};

struct __attribute__((packed)) BaseStatus {
  StatusType type;
  BaseStatus(StatusType t)
      : type(t) {
  }
};

class PeriphericalClient {
 public:
  static bool sendRequest(uint8_t module, BaseRequest* request, uint8_t length);
  static bool requestStatus(uint8_t module, BaseStatus* status, uint8_t length);
};

class PeriphericalService {
 public:
  PeriphericalService(void (*statusSender)());

  virtual void setup(uint8_t module);
  virtual void loop();

  virtual void processRequest(BaseRequest* request) = 0;
  
  static bool sendStatus(BaseStatus* status, uint8_t length);
};

/* sound api */

enum class PlayerState : uint16_t {
  UNKNOWN = 0,
  IDLE = 1,
  PLAYING = 2
};

struct __attribute__((packed)) InitSoundRequest : public BaseRequest {
  InitSoundRequest(uint16_t id)
      : BaseRequest(RequestType::SOUND_INIT),
        clientId(id) {
  }
  uint16_t clientId;
};

struct __attribute__((packed)) PlaySoundRequest : public BaseRequest {
  PlaySoundRequest(uint16_t id)
      : BaseRequest(RequestType::SOUND_PLAY),
        trackId(id) {
  }
  uint16_t trackId;
};

struct __attribute__((packed)) StopSoundRequest : public BaseRequest {
  StopSoundRequest()
      : BaseRequest(RequestType::SOUND_STOP) {
  }
};

class SoundStatusHandler {
 public:
  virtual bool playerStateChanged(PlayerState state) = 0;
};

class ActivitySoundClient : public PeriphericalClient {
 public:
  ActivitySoundClient(Scheduler* scheduler);
  void init(uint16_t clientId);
  void play(uint16_t trackId, SoundStatusHandler* statusHandler = nullptr);
  void stop();
  void setStatusHandler(SoundStatusHandler* handler);
};

struct __attribute__((packed)) SoundStatus : public BaseStatus {
  SoundStatus(PlayerState playerState = PlayerState::UNKNOWN)
      : BaseStatus(StatusType::SOUND_STATUS),
        playerState(playerState) {
  }
  PlayerState playerState;
};

/* display api */

struct __attribute__((packed)) InitDisplayRequest : public BaseRequest {
  InitDisplayRequest(uint16_t id)
      : BaseRequest(RequestType::DISPLAY_INIT),
        clientId(id) {
  }
  uint16_t clientId;
};

struct __attribute__((packed)) ClearDisplayRequest : public BaseRequest {
  ClearDisplayRequest()
      : BaseRequest(RequestType::DISPLAY_CLEAR) {
  }
};

/* 32 - 2 (request-type) - 16 (name-length) - 1 (append-flag) */
#define DISPLAY_PARAM_VALUE_BUFFER_SIZE 13

struct __attribute__((packed)) DisplayParameterRequest : public BaseRequest {
  DisplayParameterRequest(const char* name, const char* value, bool append)
      : BaseRequest(RequestType::DISPLAY_PARAMETER) {
    strlcpy(this->name, name, ID_BUFFER_SIZE);
    strlcpy(this->value, value, DISPLAY_PARAM_VALUE_BUFFER_SIZE);
    this->append = append;
  }
  char name[ID_BUFFER_SIZE];
  byte append;
  char value[DISPLAY_PARAM_VALUE_BUFFER_SIZE];
};

struct __attribute__((packed)) DisplayMessageRequest : public BaseRequest {
  DisplayMessageRequest(const char* id)
      : BaseRequest(RequestType::DISPLAY_MESSAGE) {
    strlcpy(messageId, id, ID_BUFFER_SIZE);
  }
  char messageId[ID_BUFFER_SIZE];
};

struct __attribute__((packed)) DisplayBitmapRequest : public BaseRequest {
  DisplayBitmapRequest(const char* id)
      : BaseRequest(RequestType::DISPLAY_BITMAP) {
    strlcpy(bitmapId, id, ID_BUFFER_SIZE);
  }
  char bitmapId[ID_BUFFER_SIZE];
};

struct __attribute__((packed)) DisplayStatus : public BaseStatus {
  DisplayStatus()
      : BaseStatus(StatusType::DISPLAY_STATUS) {
  }
};

class ActivityDisplayClient : public PeriphericalClient {
 public:
  ActivityDisplayClient();
  void init(uint16_t clientId);
  void clearDisplay();
  void putParam(const char* name, uint16_t value);
  void putParam(const char* name, const char* value);
  void displayMessage(const char* messageId);
  void displayBitmap(const char* bitmapId);
};

/* numpad api */

class ActivityNumpadClient {
 private:
  I2CKeyPad keyPad;

 public:
  ActivityNumpadClient();
  void init(uint16_t clientId);
  char getChar();
  int16_t readString(char* buffer, uint8_t length, int16_t timeout = -1, char until = '#');
};

/* gadget base */

class GadgetBase {
 public:
  Scheduler scheduler;
  virtual void setup();
  virtual void loop();
};

#endif