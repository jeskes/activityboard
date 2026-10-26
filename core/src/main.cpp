#include <TaskScheduler.h>
#include <WiFi.h>

#include "const.h"
#include "coreservice.h"

CoreService coreService;

void setup() {
  WiFi.mode(WIFI_OFF);
  btStop();
  // setCpuFrequencyMhz(80);

  coreService.setup(CORE_MODULE_ID);

  coreService.display.drawBitmap(STARTUP_BITMAP, 0, 0);
  coreService.player.play(SYSTEM_CLIENT_ID, STARTUP_TRACK_ID);
}

void loop() {
  coreService.loop();
}
