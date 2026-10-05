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

#define ID_BUFFER_SIZE 26
#define PARAM_BUFFER_SIZE 10
#define PRINT_BUFFER_SIZE 128
#define REQUEST_BUFFER_SIZE 32

/* common operations */

using ModuleId = uint8_t;

class ActivityBoard {
 public:
  void setup();
  void setup(ModuleId moduleId);
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

using ClientId = uint8_t;

const ClientId SYSTEM_CLIENT_ID = 10;

enum class RequestType : uint16_t {
  UNKNOWN = 0,
  SOUND_PLAY = 0x1001,
  SOUND_STOP = 0x1002,
  DISPLAY_CLEAR = 0x2001,
  DISPLAY_PARAMETER = 0x2002,
  DISPLAY_MESSAGE = 0x2003,
  DISPLAY_BITMAP = 0x2004,
  DISPLAY_MENU = 0x2005,
};

enum class StatusType : uint16_t {
  UNKNOWN = 0,
  SOUND_STATUS = 0x1001,
  DISPLAY_STATUS = 0x2001,
};

struct __attribute__((packed)) BaseRequest {
  ClientId client;
  RequestType type;
  BaseRequest(ClientId client, RequestType t)
      : client(client),
        type(t) {
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
  static bool sendRequest(ModuleId module, BaseRequest* request, uint8_t length);
  static bool requestStatus(ModuleId module, BaseStatus* status, uint8_t length);

 protected:
  PeriphericalClient(ClientId client);
  ClientId client;
};

class PeriphericalService {
 public:
  PeriphericalService(void (*statusSender)());

  virtual void setup(ModuleId module);
  virtual void loop();

  virtual void processRequest(BaseRequest* request) = 0;

  static bool sendStatus(BaseStatus* status, uint8_t length);
};

/* sound api */

using TrackId = uint16_t;

enum class PlayerState : uint16_t {
  UNKNOWN = 0,
  IDLE = 1,
  PLAYING = 2
};

struct __attribute__((packed)) PlaySoundRequest : public BaseRequest {
  PlaySoundRequest(ClientId client, TrackId track)
      : BaseRequest(client, RequestType::SOUND_PLAY),
        track(track) {
  }
  TrackId track;
};

struct __attribute__((packed)) StopSoundRequest : public BaseRequest {
  StopSoundRequest(ClientId client)
      : BaseRequest(client, RequestType::SOUND_STOP) {
  }
};

class SoundStatusHandler {
 public:
  virtual bool playerStateChanged(PlayerState state) = 0;
};

class ActivitySoundClient : public PeriphericalClient {
 public:
  ActivitySoundClient(ClientId client, Scheduler& scheduler);
  void play(TrackId track, SoundStatusHandler* statusHandler = nullptr);
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

using ParameterId = const char*;
using MessageId = const char*;
using BitmapId = const char*;
using MenuId = const char*;

struct __attribute__((packed)) ClearDisplayRequest : public BaseRequest {
  ClearDisplayRequest(ClientId client)
      : BaseRequest(client, RequestType::DISPLAY_CLEAR) {
  }
};

/* 32 - 1 (client) - 2 (request-type) - 10 (id-length) - 1 (append-flag) - 2 terminating 0 */

#define DISPLAY_PARAM_VALUE_BUFFER_SIZE 16

struct __attribute__((packed)) DisplayParameterRequest : public BaseRequest {
  DisplayParameterRequest(ClientId client, ParameterId id, const char* value, bool append)
      : BaseRequest(client, RequestType::DISPLAY_PARAMETER) {
    strlcpy(this->id, id, PARAM_BUFFER_SIZE);
    strlcpy(this->value, value, DISPLAY_PARAM_VALUE_BUFFER_SIZE);
    this->append = append;
  }
  char id[PARAM_BUFFER_SIZE];
  byte append;
  char value[DISPLAY_PARAM_VALUE_BUFFER_SIZE];
};

struct __attribute__((packed)) DisplayMessageRequest : public BaseRequest {
  DisplayMessageRequest(ClientId client, MessageId id)
      : BaseRequest(client, RequestType::DISPLAY_MESSAGE) {
    strlcpy(messageId, id, ID_BUFFER_SIZE);
  }
  char messageId[ID_BUFFER_SIZE];
};

struct __attribute__((packed)) DisplayBitmapRequest : public BaseRequest {
  DisplayBitmapRequest(ClientId client, BitmapId id)
      : BaseRequest(client, RequestType::DISPLAY_BITMAP) {
    strlcpy(bitmapId, id, ID_BUFFER_SIZE);
  }
  char bitmapId[ID_BUFFER_SIZE];
};

struct __attribute__((packed)) DisplayMenuRequest : public BaseRequest {
  DisplayMenuRequest(ClientId client, MenuId id)
      : BaseRequest(client, RequestType::DISPLAY_MENU) {
    strlcpy(menuId, id, ID_BUFFER_SIZE);
  }
  char menuId[ID_BUFFER_SIZE];
};

struct __attribute__((packed)) DisplayStatus : public BaseStatus {
  DisplayStatus()
      : BaseStatus(StatusType::DISPLAY_STATUS) {
  }
};

class ActivityDisplayClient : public PeriphericalClient {
 public:
  ActivityDisplayClient(ClientId clientId);
  void clearDisplay();
  void putParam(const char* name, uint16_t value);
  void putParam(const char* name, const char* value);
  void displayMessage(MessageId messageId);
  void displayBitmap(BitmapId bitmapId);
  void displayMenu(MenuId menuId);
};

/* numpad api */

class ActivityNumpadClient {
 private:
  I2CKeyPad keyPad;

 public:
  ActivityNumpadClient();
  void setup();
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