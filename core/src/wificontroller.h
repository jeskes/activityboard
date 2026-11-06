#pragma once

#include <TaskSchedulerDeclarations.h>
#include <WebServer.h>

#include "extrasexpansion.h"

enum WifiState {
  WIFI_IDLE,
  WIFI_START_AP,
  WIFI_CHECK_AP,
  WIFI_START_SERVER,
  WIFI_SERVER_RUNNING
};

class WifiController {
 public:
  WifiController(Scheduler& scheduler, ExtrasExpansion& extras);

  void setup();

 private:
  WifiState currentState;

  WebServer server;
  Scheduler& scheduler;
  ExtrasExpansion& extras;

  void startStop();

  Task wifiTask;
  void processWifiState();
};
