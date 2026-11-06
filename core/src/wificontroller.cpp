#include "wificontroller.h"

#include <WiFi.h>
#include <base/base.h>

#include "const.h"

WifiController::WifiController(Scheduler& scheduler, ExtrasExpansion& extras)
    : server(80),
      extras(extras),
      scheduler(scheduler),
      wifiTask(WIFI_ACTIVATION_POLLING, TASK_FOREVER, [this]() { this->processWifiState(); }) {
}

void WifiController::setup() {
  extras.showLed(EXTRAS_WIFI_LED, false);
  extras.addHandler(EXTRAS_WIFI_BUTTON, [&]() { startStop(); });

  scheduler.addTask(wifiTask);
  wifiTask.enable();

  server.on("/", [&]() {
    server.send(200, "text/plain", "Hallo vom ActivityBoard!");
  });
}

void WifiController::startStop() {
  if (currentState == WIFI_IDLE) {
    LOG("Asynchroner Start des Hotspots initiiert...");
    currentState = WIFI_START_AP;
    wifiTask.enable();
  }
  else {
    LOG("WiFi & Webserver werden heruntergefahren...");
    wifiTask.disable();
    server.stop();
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
      if (WiFi.softAPIP() != IPAddress(0, 0, 0, 0)) {
		IPAddress myIP = WiFi.softAPIP();
        LOG("WLAN-Hotspot steht! IP: %s", WiFi.softAPIP().toString().c_str());
        LOG("Hotspot erfolgreich gestartet!");
        LOG("SSID: MeinActivityBoard");
        LOG("Verbinde dich und öffne im Browser: http://%s", myIP.toString().c_str());
        currentState = WIFI_START_SERVER;
      }
      extras.toggleLed(EXTRAS_WIFI_LED);
      break;

    case WIFI_START_SERVER:
      LOG("Starte asynchronen Webserver...");
      server.begin();  // Startet den Webserver
      currentState = WIFI_SERVER_RUNNING;
      break;

    case WIFI_SERVER_RUNNING:
      server.handleClient();
      extras.showLed(EXTRAS_WIFI_LED, true);
      wifiTask.setInterval(5);
      break;

    default:
      break;
  }
}
