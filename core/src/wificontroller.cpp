#include "wificontroller.h"

#include <FS.h>
#include <SD.h>
#include <WiFi.h>
#include <base/base.h>
#include <uri/UriBraces.h>

#include "const.h"

static IPAddress ap_ip(10, 0, 0, 1);
static IPAddress ap_gateway(10, 0, 0, 1);
static IPAddress ap_subnet(255, 255, 255, 0);

WifiController::WifiController(ClientContexts contexts)
    : state(State::STOPPED),
      server(80),
      webSocket(81),
      contexts(contexts) {
}

void WifiController::setup() {
  server.on("/", [&]() { handleRoot(); });

  server.on(UriBraces("/{}/images/{}"), [&]() {
    String s = "/";
    s.concat(server.pathArg(0));
    s.concat("/images/");
    s.concat(server.pathArg(1));
    handleImage(s.c_str());
  });

  webSocket.onEvent([&](uint8_t num, WStype_t type, uint8_t* payload, size_t length) { webSocketEvent(num, type, payload, length); });
}

void WifiController::setHandler(WifiStateChangeHandler handler) {
  this->handler = handler;
}

WebState WifiController::fetchState() {
  WebState webState;
  webState.active = state == State::RUNNING;
  webState.age = webInputTimestamp ? millis() - webInputTimestamp : 0;
  webState.input = webInput;
  webInput = StdInput::NONE;
  webInputTimestamp = 0;
  return webState;
}

void WifiController::configureClient(ClientId client) {
  this->client = client;
  if (webSocketConnected) {
    sendWebConfig();
  }
}

void WifiController::startStop() {
  if (state == State::RUNNING) {
    stop();
  }
  else if (state == State::STOPPED) {
    start();
  }
}

void WifiController::start() {
  if (state != State::STOPPED) {
    return;
  }

  state = State::STARTING;
  LOG("start wifi");

  webSocketNumber = 0xff;
  webSocketConnected = false;
  webInput = StdInput::NONE;

  WiFi.disconnect(true);
  delay(100);
  WiFi.persistent(false);

  WiFi.mode(WIFI_AP);
  if (!WiFi.softAP(WIFI_SSID, WIFI_PASSPHRASE)) {
    LOG("cannot start wifi access point");
    return;
  }

  delay(150);
  WiFi.softAPConfig(ap_ip, ap_gateway, ap_subnet);
  WiFi.setSleep(false);

  if (WiFi.softAPIP() == IPAddress(0, 0, 0, 0)) {
    LOG("cannot start wifi ap ip");
    return;
  }

  server.begin();
  webSocket.begin();

  state = State::RUNNING;

  LOG("wifi started: ip=%s", WiFi.softAPIP().toString().c_str());
}

void WifiController::stop() {
  if (state != State::RUNNING) {
    return;
  }

  state = STOPPING;
  LOG("stop wifi");

  server.stop();
  webSocket.close();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_OFF);

  state = State::STOPPED;

  LOG("wifi stopped");
}

bool WifiController::isRunning() {
  return state == State::RUNNING;
}

void WifiController::loop() {
  if (isRunning()) {
    server.handleClient();
    webSocket.loop();
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
  server.send(200, "image/jpeg", "");

  uint8_t buffer[512];  // 512 Byte Puffer (schont den RAM des ESP32)
  while (imgFile.available()) {
    size_t bytesRead = imgFile.read(buffer, sizeof(buffer));
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
