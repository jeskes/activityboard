#include "gadget.h"

void GadgetBase::setup(uint8_t led) {
  if (led) {
    pinMode(led, OUTPUT);
    activityControls.blink(led, GADGET_INDICATE_STARTUP_CYCLES, GADGET_INDICATE_STARTUP_BLINKRATE);
    activityControls.blink(led, 1, GADGET_INDICATE_STARTUP_CYCLES * GADGET_INDICATE_STARTUP_BLINKRATE);
  }
  else {
    delay(GADGET_STARTUP_DELAY);
  }
  activityBoard.setup();
  LOG("gadget setup done.");
}

void GadgetBase::loop() {
  scheduler.execute();
}
