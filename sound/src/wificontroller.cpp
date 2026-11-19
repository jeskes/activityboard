#include "wificontroller.h"

#include <base/base.h>
#include <esp_wifi.h>

#include "const.h"

static IPAddress local_ip(10, 0, 0, 50);
static IPAddress gateway(10, 0, 0, 1);
static IPAddress subnet(255, 255, 255, 0);

WifiController::WifiController()
    : started(false) {
}

void WifiController::setup() {
  ftpServer.setLocalIp(local_ip);
}

void WifiController::loop() {
  for (int i = 0; i < 20; i++) {
    ftpServer.handleFTP();
  }
}

bool WifiController::start() {
  if (started) {
    return false;
  }

  /*
    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i) {
      LOG("found wlan: %s (RSSI: %d, Ch: %d)", WiFi.SSID(i).c_str(), WiFi.RSSI(i), WiFi.channel(i));
    }
    WiFi.scanDelete();
  */

  WiFi.disconnect();
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  esp_wifi_set_bandwidth(WIFI_IF_STA, WIFI_BW_HT20);
  WiFi.setMinSecurity(WIFI_AUTH_WPA_PSK);

  WiFi.onEvent(
      [](WiFiEvent_t event, WiFiEventInfo_t info) {
        LOG("wifi connect error");
        LOG("WiFi-Disconnect-Reason: %d", info.wifi_sta_disconnected.reason);
      },
      ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  WiFi.config(local_ip, gateway, subnet);
  WiFi.begin(WIFI_SSID, WIFI_PASSPHRASE);
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
  WiFi.setSleep(false);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 10000) {
    delay(200);
  }

  if (WiFi.status() != WL_CONNECTED) {
    LOG("could not start wifi: status=%d", WiFi.status());
    return false;
  }

  ftpServer.begin(WIFI_USERNAME, WIFI_PASSWORD);
  started = true;

  LOG("wifi and ftp started: ip=%s", WiFi.localIP().toString().c_str());

  return true;
}

bool WifiController::stop() {
  if (!started) {
    return false;
  }

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  started = false;
  LOG("wifi stopped");
  return true;
}
