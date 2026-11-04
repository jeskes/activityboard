#pragma once

#include "requestqueue.h"

class BaseStatus;

class PeriphericalService : public RequestHandler {
 public:
  PeriphericalService();

  virtual void setup(ModuleId module);
  virtual void loop();

  virtual void processRequest(BaseRequest* request) = 0;

  static bool writeStatus(BaseStatus* status, uint8_t length);
};
