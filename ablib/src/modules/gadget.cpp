#include "gadget.h"

void GadgetBase::setup() {
  delay(1000);
  activityBoard.setup();
  LOG("gadget setup done.");
}

void GadgetBase::loop() {
  scheduler.execute();
}
