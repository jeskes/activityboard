#include <esp_wifi.h>

#include "MaintenanceController.h"

MaintenanceController* MaintenanceController::instance = nullptr;

MaintenanceController::MaintenanceController(Configuration& configuration)
    : configuration(configuration),
      state(State::STOPPED) {
  instance = this;
}

void MaintenanceController::startStop() {
  if (state == State::RUNNING) {
    stop();
  }
  else if (state == State::STOPPED) {
    start();
  }
}

void MaintenanceController::start() {
  if (state != State::STOPPED) {
    return;
  }

  state = State::STARTING;
  LOG("start maintenance");

  WiFi.disconnect(true);
  delay(100);
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_bandwidth(WIFI_IF_STA, configuration.wifiBandwidth);

  WiFi.setHostname(configuration.hostname);
  WiFi.begin(configuration.wifiSsid, configuration.wifiPassphrase);
  WiFi.setTxPower(configuration.wifiPower);
  WiFi.setSleep(false);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && (millis() - startAttempt < configuration.wifiTimeoutMs)) {
    delay(200);
  }

  if (WiFi.status() != WL_CONNECTED) {
    LOG("cannot connect to wlan; ssid=%s'", configuration.wifiSsid);
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    state = State::STOPPED;
    return;
  }

  delay(100);

  ArduinoOTA.setHostname(configuration.hostname);
  if (configuration.otaPassword && strlen(configuration.otaPassword) > 0) {
    ArduinoOTA.setPassword(configuration.otaPassword);
  }

  ArduinoOTA.onStart([this]() { otaActive = true; });
  ArduinoOTA.onEnd([this]() { otaActive = false; });
  ArduinoOTA.onError([this](ota_error_t) { otaActive = false; });
  ArduinoOTA.begin();

  ftpServer.setCallback(ftpCallback);
  ftpServer.setTransferCallback(ftpTransferCallback);
  ftpServer.begin(configuration.ftpUsername, configuration.ftpPassword);

  MDNS.addService("ftp", "tcp", 21);

  state = State::RUNNING;

  LOG("maintenance started: id=%s (%s.local)\n", WiFi.localIP().toString().c_str(), configuration.hostname);
}

void MaintenanceController::stop() {
  if (state != State::RUNNING) {
    return;
  }

  state = State::STOPPING;
  LOG("stop maintenance");

  MDNS.end();
  WiFi.disconnect(true);
  delay(100);
  WiFi.mode(WIFI_OFF);

  ftpActive = false;
  otaActive = false;

  state = State::STOPPED;

  LOG("maintenance stopped");
}

void MaintenanceController::loop() {
  if (state != State::RUNNING)
    return;

  ArduinoOTA.handle();

  int ftpLoops = ftpActive ? 20 : 1;
  do {
    ftpServer.handleFTP();
  } while (--ftpLoops > 0);
}

bool MaintenanceController::isRunning() const {
  return state == State::RUNNING;
}

bool MaintenanceController::isBusy() const {
  return ftpActive || otaActive;
}

void MaintenanceController::ftpCallback(FtpOperation op, uint32_t freeSpace, uint32_t totalSpace) {
  if (!instance || !instance->isRunning()) {
    return;
  }
  if (op == FTP_CONNECT) {
    LOG("ftp conntected");
  }
  else if (op == FTP_DISCONNECT) {
    LOG("ftp disconntected");
    instance->ftpActive = false;
  }
}

void MaintenanceController::ftpTransferCallback(FtpTransferOperation op, const char* name, uint32_t transferredSize) {
  if (!instance || !instance->isRunning()) {
    return;
  }
  if (op == FTP_UPLOAD_START || op == FTP_DOWNLOAD_START) {
    instance->ftpActive = true;
    LOG("ftp transfer start: op=%d", op);
  }
  else if (op == FTP_TRANSFER_STOP || op == FTP_TRANSFER_ERROR) {
    instance->ftpActive = false;
    LOG("ftp transfer stop: op=%d", op);
  }
}