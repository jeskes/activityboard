#pragma once

#include <Arduino.h>

#include "../base/base.h"

/* ========================================================================= */
/*                                                   peripherical api (base) */
/* ========================================================================= */

using ClientId = uint8_t;

const ClientId SYSTEM_CLIENT_ID = 10;

enum class RequestType : uint16_t {
  UNKNOWN = 0,
  SOUND_PLAY = 0x1001,
  SOUND_STOP = 0x1002,
  DISPLAY_INIT = 0x2000,
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
  bool sendRequest(ModuleId module, BaseRequest* request, uint8_t length);
  static bool requestStatus(ModuleId module, BaseStatus* status, uint8_t length);

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
