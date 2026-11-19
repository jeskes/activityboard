#pragma once

#include <TaskSchedulerDeclarations.h>

#undef NETWORK_ENC28J60
#undef NETWORK_ESP32
#undef NETWORK_ESP32_ETH
#undef NETWORK_ESP8266
#undef NETWORK_ESP8266_ASYNC
#undef NETWORK_W5100
#include <WebServer.h>

#undef NETWORK_ENC28J60
#undef NETWORK_ESP32
#undef NETWORK_ESP32_ETH
#undef NETWORK_ESP8266
#undef NETWORK_ESP8266_ASYNC
#undef NETWORK_W5100
#include <WebSocketsServer.h>

#include <modules/requests.h>

#include "clientcontext.h"
#include "extrasexpansion.h"

using WifiStateChangeHandler = std::function<void()>;

class WifiController {
 public:
  WifiController(ClientContexts contexts);

  enum State {
    STOPPED,
    STARTING,
    RUNNING,
	STOPPING
  };

  void setup();
  void loop();

  void startStop();
  void start();
  void stop();

  bool isRunning();

  void configureClient(ClientId client);
  WebState fetchState();

  void setHandler(WifiStateChangeHandler handler);

 private:
  State state;
  WifiStateChangeHandler handler;

  WebServer server;
  WebSocketsServer webSocket;

  ClientId client;
  bool webSocketConnected;
  uint8_t webSocketNumber;

  ClientContexts& contexts;

  void handleRoot();
  void handleImage(const char* path);

  void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length);
  void sendWebConfig();
  void processWebMessage(uint8_t* payload, size_t length);

  Input webInput;
  unsigned long webInputTimestamp;
};
