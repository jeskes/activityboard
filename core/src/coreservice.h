#pragma once

#include <SPI.h>
#include <modules/modules.h>

#include "clientcontext.h"
#include "display.h"
#include "player.h"
#include "storage/storage.h"
#include "taskrunner.h"
#include "ledring.h"

class CoreService : public PeriphericalService {
 public:
  CoreService();

  LedRing ledRing;
  Player player;
  Storage storage;
  Display display;
  ClientContexts contexts;

  TaskRunner runner;

  void setup(uint8_t module) override;
  void loop() override;

  void processRequest(BaseRequest* request) override;

  void handleRequest(InitCoreRequest* request);
  void handleRequest(ParameterRequest* request);
  void handleRequest(PlaySoundRequest* request);
  void handleRequest(StopSoundRequest* request);
  void handleRequest(ClearDisplayRequest* request);
  void handleRequest(DisplayMessageRequest* request);
  void handleRequest(DisplayMenuRequest* request);
  void handleRequest(DisplayBitmapRequest* request);

 private:
  static void publishStatus();
};
