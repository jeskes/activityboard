#pragma once

#include <TaskSchedulerDeclarations.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <modules/requests.h>

#include "clientcontext.h"
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
  WifiController(Scheduler& scheduler, ExtrasExpansion& extras, ClientContexts contexts);

  void setup();

  void configureClient(ClientId client);
  WebState fetchState();

 private:
  WifiState currentState;

  WebServer server;
  WebSocketsServer webSocket;

  ClientId client;
  bool webSocketConnected;
  uint8_t webSocketNumber;

  Scheduler& scheduler;
  ExtrasExpansion& extras;
  ClientContexts& contexts;

  void startStop();

  Task wifiTask;
  void processWifiState();

  void handleRoot();
  void handleImage(const char* path);

  void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length);
  void sendWebConfig();
  void processWebMessage(uint8_t* payload, size_t length);

  Input webInput;
  unsigned long webInputTimestamp;
};
