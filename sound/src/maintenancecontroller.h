#pragma once

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <SD.h>
#include <WiFi.h>
#include <base/base.h>

#include <functional>

#include "ftpserver.h"

class MaintenanceController {
 public:
  struct Configuration {
    const char* hostname;
    const char* wifiSsid = MAINTENANCE_SSID;
    const char* wifiPassphrase = MAINTENANCE_PASSPHRASE;
    const wifi_bandwidth_t wifiBandwidth = WIFI_BW_HT40; /* use WIFI_BW_HT20 on ESP32-C3 */
    const wifi_power_t wifiPower = WIFI_POWER_19_5dBm;   /* use WIFI_POWER_11dBm on ESP32-C3 */
    const char* ftpUsername = MAINTENANCE_FTP_USERNAME;
    const char* ftpPassword = MAINTENANCE_FTP_PASSWORD;
    const char* otaPassword = MAINTENANCE_OTA_PASSWORD;
    uint32_t wifiTimeoutMs = WIFI_TIMEOUT;
  };

  enum class State {
    STOPPED,
    STARTING,
    RUNNING,
    STOPPING
  };

  MaintenanceController(Configuration& configuration);

  void loop();

  void startStop();
  void start();
  void stop();

  bool isRunning() const;
  bool isBusy() const;

 private:
  FTPServer ftpServer;
  Configuration& configuration;

  State state;
  volatile bool ftpActive = false;
  volatile bool otaActive = false;

  static MaintenanceController* instance;
};
