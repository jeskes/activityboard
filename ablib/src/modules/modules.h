#pragma once

#include <Arduino.h>

#include "../base/base.h"

/* ========================================================================= */
/*                                                   peripherical api (base) */
/* ========================================================================= */

using ClientId = uint8_t;

enum class RequestType : uint16_t {
  UNKNOWN = 0,

  INIT = 0x0001,
  PARAMETER = 0x0002,
  STATUS = 0x0003,
  PLAY_SOUND = 0x0010,
  STOP_SOUND = 0x0011,
  CLEAR_DISPLAY = 0x0020,
  DISPLAY_MESSAGE = 0x0021,
  DISPLAY_BITMAP = 0x0022,
  DISPLAY_MENU = 0x0023,
  READ_DATA = 0x0024,
};

enum class StatusType : uint16_t {
  UNKNOWN = 0,
  CORE_STATUS = 0x0001,
  CORE_DATA = 0x0002
};

struct __attribute__((packed)) BaseRequest {
  ClientId client;
  RequestType type;
  BaseRequest(ClientId client, RequestType t)
      : client(client),
        type(t) {
  }
};

struct __attribute__((packed)) StatusRequest : public BaseRequest {
  StatusRequest(ClientId client, StatusType type)
      : BaseRequest(client, RequestType::STATUS),
        type(type) {
  }
  StatusType type;
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
  static bool requestStatus(ModuleId module, ClientId client, StatusType type, BaseStatus* status, uint8_t length);
  static bool requestStatus(ModuleId module, ClientId client, StatusType type, uint8_t* status, uint8_t length, unsigned long receiveDelay = REQUEST_STATUS_RECEIVE_DELAY);

 protected:
  PeriphericalClient(ClientId client, Scheduler& scheduler);
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

/* ========================================================================= */
/*                                                               gadget base */
/* ========================================================================= */

class GadgetBase {
 public:
  Scheduler scheduler;
  virtual void setup();
  virtual void loop();
};
