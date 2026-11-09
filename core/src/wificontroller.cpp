#include "wificontroller.h"

#include <FS.h>
#include <SD.h>
#include <WiFi.h>
#include <base/base.h>
#include <uri/UriBraces.h>

#include "const.h"

WifiController::WifiController(Scheduler& scheduler, ExtrasExpansion& extras, ClientContexts contexts)
    : server(80),
      webSocket(81),
      extras(extras),
      contexts(contexts),
      scheduler(scheduler),
      wifiTask(WIFI_ACTIVATION_POLLING, TASK_FOREVER, [this]() { this->processWifiState(); }) {
}

void WifiController::setup() {
  extras.showLed(EXTRAS_WIFI_LED, false);
  extras.addHandler(EXTRAS_WIFI_BUTTON, [&]() { startStop(); });

  scheduler.addTask(wifiTask);
  wifiTask.enable();

  server.on("/", [&]() { handleRoot(); });

  server.on(UriBraces("/{}/bitmaps/{}"), [&]() {
    String s = "/";
    s.concat(server.pathArg(0));
    s.concat("/bitmaps/");
    s.concat(server.pathArg(1));
    handleImage(s.c_str());
  });

  webSocket.onEvent([&](uint8_t num, WStype_t type, uint8_t* payload, size_t length) { webSocketEvent(num, type, payload, length); });
}

void WifiController::onStateChange(WifiStateChangeHandler handler) {
  this->handler = handler;
}

WebState WifiController::fetchState() {
  WebState state;
  state.active = currentState == WifiState::WIFI_SERVER_RUNNING;
  state.age = webInputTimestamp ? millis() - webInputTimestamp : 0;
  state.input = webInput;
  webInput = StdInput::NONE;
  webInputTimestamp = 0;
  return state;
}

void WifiController::configureClient(ClientId client) {
  this->client = client;
  if (webSocketConnected) {
    sendWebConfig();
  }
}

void WifiController::startStop() {
  if (currentState == WIFI_IDLE) {
    LOG("start wifi...");
    currentState = WIFI_START_AP;
    webSocketNumber = 0xff;
    webSocketConnected = false;
    webInput = StdInput::NONE;
    wifiTask.enable();
      wifiTask.setInterval(WIFI_ACTIVATION_POLLING);
  }
  else {
    LOG("shutdown wifi");
    wifiTask.disable();
    server.stop();
    webSocket.close();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    extras.showLed(EXTRAS_WIFI_LED, false);
    currentState = WIFI_IDLE;
  }
}

void WifiController::processWifiState() {
  switch (currentState) {
    case WIFI_START_AP:
      WiFi.disconnect(true);
      WiFi.mode(WIFI_AP);
      WiFi.softAP("fizzbizz", "fizzbizz");
      extras.toggleLed(EXTRAS_WIFI_LED);
      currentState = WIFI_CHECK_AP;
      break;

    case WIFI_CHECK_AP:
      extras.toggleLed(EXTRAS_WIFI_LED);
      if (WiFi.softAPIP() != IPAddress(0, 0, 0, 0)) {
        IPAddress ip = WiFi.softAPIP();
        LOG("wifi started ip=%s", WiFi.softAPIP().toString().c_str());
        LOG("connect to http://%s", ip.toString().c_str());
        currentState = WIFI_START_SERVER;
      }
      break;

    case WIFI_START_SERVER:
      server.begin();
      webSocket.begin();
      currentState = WIFI_SERVER_RUNNING;
      break;

    case WIFI_SERVER_RUNNING:
      server.handleClient();
      webSocket.loop();
      extras.showLed(EXTRAS_WIFI_LED, true);
      wifiTask.setInterval(WIFI_ACTIVE_POLLING);
      break;

    default:
      break;
  }
}

void WifiController::handleRoot() {
  LOG("handle root");
  File htmlFile = SD.open("/10/website/index.html", FILE_READ);
  if (!htmlFile) {
    server.send(404, "text/plain", "index.html not found");
    return;
  }
  server.streamFile(htmlFile, "text/html");
  htmlFile.close();
}

void WifiController::handleImage(const char* path) {
  LOG("handle image: %s", path);
  File imgFile = SD.open(path, FILE_READ);
  if (!imgFile) {
    server.send(404, "text/plain", "image not found");
    return;
  }
  if (imgFile.size() == 0) {
    LOG("file %s is empty", path);
    imgFile.close();
    server.send(500, "text/plain", "image is empty");
    return;
  }

  server.setContentLength(imgFile.size());
  server.send(200, "image/bmp", "");

  uint8_t buffer[512];  // 512 Byte Puffer (schont den RAM des ESP32)
  while (imgFile.available()) {
    size_t bytesRead = imgFile.read(buffer, sizeof(buffer));
    LOG("send bytes: %d", bytesRead);
    if (bytesRead > 0) {
      server.client().write(buffer, bytesRead);
    }
  }

  imgFile.close();
}

void WifiController::webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  DUMP(payload, length);
  if (type == WStype_CONNECTED) {
    webSocketNumber = num;
    webSocketConnected = true;
    LOG("websocket connected: socket=%d", webSocketNumber);
    if (client) {
      sendWebConfig();
    }
  }
  else if (type == WStype_DISCONNECTED) {
    webSocketNumber = 0xff;
    webSocketConnected = false;
  }
  else if (type == WStype_TEXT) {
    processWebMessage(payload, length);
  }
}

void WifiController::sendWebConfig() {
  if (!webSocketConnected || !client) {
    LOG("no websocket connection or client");
    return;
  }

  LOG("send webconfig: socket=%d, client=%02d", webSocketNumber, client);

  char path[PATH_BUFFER_SIZE];
  sprintf(path, "/%02d/website/config.json", client);
  File configFile = SD.open(path, FILE_READ);
  if (!configFile) {
    LOG("config.json not found : path=%s", path);
    return;
  }

  String reply = "CONFIG:";
  while (configFile.available()) {
    reply += (char)configFile.read();
  }
  configFile.close();

  webSocket.sendTXT(webSocketNumber, reply);
  LOG("config.json sent: socket=%d, path=%s", webSocketNumber, path);
}

void WifiController::processWebMessage(uint8_t* payload, size_t length) {
  String msg = String((char*)payload);
  LOG("received web message : %s", msg.c_str());
  if (msg.startsWith("INPUT:")) {
    webInput = msg.substring(6).toInt();
    webInputTimestamp = millis();
    if (handler) {
      handler();
    }
  }
}
