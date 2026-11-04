#pragma once

#include "requests.h"

class PeriphericalClient {
 public:
  static bool sendRequest(ModuleId module, BaseRequest* request, uint8_t length);
  static bool requestStatus(ModuleId module, ClientId client, StatusType statusType, BaseStatus* status, uint8_t length);

 protected:
  PeriphericalClient(ClientId client, Scheduler& scheduler);
  ClientId client;
};
