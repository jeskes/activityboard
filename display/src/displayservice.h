#pragma once

#include <SPI.h>
#include <modules/modules.h>

#include "clientcontext.h"
#include "display.h"
#include "storage/storage.h"
#include "taskrunner.h"
#include "rotaryencoder.h"

class DisplayService : public PeriphericalService {
 public:
  DisplayService();

  Storage storage;
  Display display;
  ClientContexts contexts;
  RotaryEncoderWrapper rotary;

  TaskRunner runner;

  void setup(uint8_t module) override;
  void loop() override;

  void processRequest(BaseRequest* request) override;

 private:
  static void publishStatus();

  void handleRequest(InitDisplayRequest* request);
  void handleRequest(ClearDisplayRequest* request);
  void handleRequest(DisplayParameterRequest* request);
  void handleRequest(DisplayMessageRequest* request);
  void handleRequest(DisplayMenuRequest* request);
  void handleRequest(DisplayBitmapRequest* request);
};
