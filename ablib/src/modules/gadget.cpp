#include "gadget.h"

void GadgetBase::setup(uint8_t startupLed) {
  if (startupLed) {
	pinMode(startupLed, OUTPUT);
	activityControls.blink(startupLed, 5, 100);
	activityControls.blink(startupLed, 1, 500);
  }
  else {
	  delay(1000);
  }
  activityBoard.setup();
  LOG("gadget setup done.");
}

void GadgetBase::loop() {
  scheduler.execute();
}
