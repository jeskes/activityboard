#pragma once

#include <SPI.h>
#include <TaskSchedulerDeclarations.h>
#include <maintenace/maintenancecontroller.h>
#include <modules/periphericalservice.h>

#include "clientcontext.h"
#include "display.h"
#include "extrasexpansion.h"
#include "gadgetswitcher.h"
#include "ledring.h"
#include "rotaryexpansion.h"
#include "soundcontroller.h"
#include "storage/storage.h"
#include "taskrunner.h"
#include "wificontroller.h"

class CoreService : public PeriphericalService {
 public:
  CoreService();

  RotaryExpansion rotary;
  ExtrasExpansion extras;
  GadgetSwitcher gadgetSwitcher;

  LedRing ledRing;
  Storage storage;
  Display display;
  ClientContexts contexts;
  WifiController wifiController;
  SoundController soundController;
  MaintenanceController maintenanceController;

  TaskRunner runner;
  Scheduler scheduler;

  void setup(uint8_t module) override;
  void loop() override;
  void stopAll();

  /* queued request processing */
  void processRequest(BaseRequest* request) override;
  void processRequest(InitCoreRequest* request);
  void processRequest(ParameterRequest* request);
  void processRequest(PlayTrackRequest* request);
  void processRequest(StopSoundRequest* request);
  void processRequest(ClearDisplayRequest* request);
  void processRequest(DisplayMessageRequest* request);
  void processRequest(DisplayMenuRequest* request);
  void processRequest(DisplayImageRequest* request);

  /* direct request handling */
  bool handleRequest(BaseRequest* request) override;
  bool handleRequest(StatusRequest* request);
  bool handleRequest(ReadDataRequest* request);

  /* core status processing */
  bool writeCoreStatus();
  void fireCoreStatusUpdate();
  CoreControl coreControl;
  unsigned long lastHeartbeat;

  /* wifi & maintenance */
  void startStopWifi();
  void startStopMaintenance(bool longPressed);

  /* sound control */
  void handleSoundControl(SoundControl control);
  void displayTrackImage(TrackPath path);

  /* gaget switching */
  void onGadgetSwitched(int8_t index);
};
