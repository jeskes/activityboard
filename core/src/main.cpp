#include <TaskScheduler.h>
#include <WiFi.h>

#include "const.h"
#include "coreservice.h"

CoreService coreService;
static bool startup = true;

void setup() {
  WiFi.mode(WIFI_OFF);
  btStop();

  coreService.setup(CORE_MODULE_ID);
}

void loop() {
  if (startup) {
    coreService.display.drawImage(STARTUP_IMAGE);
    startup = false;
  }
  coreService.loop();
}
