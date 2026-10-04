#include "displayservice.h"

DisplayService displayService;

void setup() {
  displayService.setup(DISPLAY_MODULE_ID);

  /* test */
  // displayService.context.init(11);
  displayService.storage.diagnostics();
  displayService.display.drawBitmap("/10/startup.bmp", 0, 0);
  // displayService.display.drawText("startup");
}

void loop() {
  displayService.loop();
}
