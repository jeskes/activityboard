#pragma once

#include <SPI.h>
#include <modules/periphericalservice.h>

#include "clientcontext.h"
#include "display.h"
#include "ledring.h"
#include "player.h"
#include "storage/storage.h"
#include "taskrunner.h"

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

  /* queued request processing */
  void processRequest(BaseRequest* request) override;
  void processRequest(InitCoreRequest* request);
  void processRequest(ParameterRequest* request);
  void processRequest(PlaySoundRequest* request);
  void processRequest(StopSoundRequest* request);
  void processRequest(ClearDisplayRequest* request);
  void processRequest(DisplayMessageRequest* request);
  void processRequest(DisplayMenuRequest* request);
  void processRequest(DisplayBitmapRequest* request);

  /* direct request handling */
  bool handleRequest(BaseRequest* request) override;
  bool handleRequest(StatusRequest* request);
  bool handleRequest(ReadDataRequest* request);

  bool writeCoreStatus();
};
