#ifndef _AB_LIB
#define _AB_LIB

#include <Arduino.h>
#include <TaskSchedulerDeclarations.h>

/* constants */

#define SOUND_MODULE_ID 0x10
#define NUMPAD_MODULE_ID 0x11
#define DISPLAY_MODULE_ID 0x12

#define WIRE_CLOCK 50000
#define WIRE_TIMEOUT 3000

#define PRINT_BUFFER_SIZE 128
#define REQUEST_BUFFER_SIZE 32

/* common operations */

class ActivityBoard {
 public:
  void init();
  void init(int moduleId);
};

extern ActivityBoard activityBoard;

class ActivityLogger {
 public:
  void serialPrintln(const char* fmt, ...);
  void serialPrintln(const __FlashStringHelper* fmt, ...);
  void serialPrintln(const char* fmt, bool isFlash, va_list args);
};

extern ActivityLogger activityLogger;

#define LOG(fmt, ...) activityLogger.serialPrintln(F(fmt), ##__VA_ARGS__)

class ActivityControls {
 public:
  void blink(int pin, int cycleCount, int timeout);
  void blink(const int pins[], int pinCount, int cycleCount, int timeout);

  bool buttonPressed(int pin);
};

extern ActivityControls activityControls;

/* peripherical api */

enum class RequestType : int {
  UNKNOWN = 0,
  SOUND_PLAY = 0x1001,
  SOUND_STOP = 0x1002
};

enum class StatusType : int {
  UNKNOWN = 0,
  SOUND_STATUS = 0x1001,
};

struct __attribute__((packed)) BaseRequest {
  RequestType type;
  BaseRequest(RequestType t) : type(t) {
  }
};

struct __attribute__((packed)) BaseStatus {
  StatusType type;
  BaseStatus(StatusType t) : type(t) {
  }
};

class PeriphericalClient {
 public:
  /* sends some request to peripherical */
  static bool sendRequest(int module, BaseRequest* request, int length);

  /* request status from peripherical */
  static bool requestStatus(int module, BaseStatus* status, int length);
};

class PeriphericalService {
 public:
  PeriphericalService();

  virtual void setup(int module);
  virtual void loop();

  /* peripherical handles request sent from master */
  virtual void processRequest(BaseRequest* request) = 0;

  /* peripherical must collect and send status to master */
  virtual void publishStatus() = 0;

 protected:
  /* peripherical sends status to master during sendStatus */
  static bool sendStatus(BaseStatus* status, int length);

 private:
  void readRequest(int length);
  BaseRequest* currentRequest = nullptr;
  byte requestBuffer[REQUEST_BUFFER_SIZE];

  /* singleton instance */
  static PeriphericalService* instance;

  /* wire callbacks*/
  static void onReceive(int length);
  static void onRequest();
};

/* sound api */

enum class PlayerState : int {
  UNKNOWN = 0,
  IDLE = 1,
  PLAYING = 2
};

struct __attribute__((packed)) PlaySoundRequest : public BaseRequest {
  PlaySoundRequest(int f, int t) : BaseRequest(RequestType::SOUND_PLAY), folder(f), track(t) {
  }
  int folder;
  int track;
};

struct __attribute__((packed)) StopSoundRequest : public BaseRequest {
  StopSoundRequest() : BaseRequest(RequestType::SOUND_STOP) {
  }
};

class SoundStatusHandler {
 public:
  virtual bool playerStateChanged(PlayerState state) = 0;
};

class ActivitySoundClient : public PeriphericalClient {
 public:
  ActivitySoundClient(Scheduler* scheduler);

  void play(int folder, int track, SoundStatusHandler* statusHandler = nullptr);
  void stop();

  void setStatusHandler(SoundStatusHandler* handler);
};

struct __attribute__((packed)) SoundStatus : public BaseStatus {
  SoundStatus() : BaseStatus(StatusType::SOUND_STATUS) {
  }
  PlayerState playerState;
};

/* gadget base */

class GadgetBase {
 public:
  Scheduler scheduler;

  virtual void setup();
  virtual void loop();
};

#endif
