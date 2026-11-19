#pragma once

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <SD.h>

#undef NETWORK_ENC28J60
#undef NETWORK_ESP32
#undef NETWORK_ESP32_ETH
#undef NETWORK_ESP8266
#undef NETWORK_ESP8266_ASYNC
#undef NETWORK_W5100
#define DEFAULT_STORAGE_TYPE_ESP32 STORAGE_SD
#include <SimpleFTPServer.h>

#include <functional>

#include "../base/base.h"

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
  FtpServer ftpServer;
  Configuration& configuration;

  State state;
  bool ftpActive = false;
  bool otaActive = false;

  static MaintenanceController* instance;
  static void ftpCallback(FtpOperation op, uint32_t freeSpace, uint32_t totalSpace);
  static void ftpTransferCallback(FtpTransferOperation op, const char* name, uint32_t transferredSize);
};