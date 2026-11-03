#pragma once

#include <SPI.h>
#include <modules/modules.h>

#include "clientcontext.h"
#include "dataprovider.h"
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

  void processRequest(BaseRequest* request) override;

  void processRequest(StatusRequest* request);
  void processRequest(InitCoreRequest* request);
  void processRequest(ParameterRequest* request);
  void processRequest(PlaySoundRequest* request);
  void processRequest(StopSoundRequest* request);
  void processRequest(ClearDisplayRequest* request);
  void processRequest(DisplayMessageRequest* request);
  void processRequest(DisplayMenuRequest* request);
  void processRequest(DisplayBitmapRequest* request);
  void processRequest(ReadDataRequest* request);

 private:
  static StatusType requestedStatus;
  static void publishStatus();
  static void publishCoreStatus();
  static void publishDataStream();

  static bool handleRequest( BaseRequest* request);
};
