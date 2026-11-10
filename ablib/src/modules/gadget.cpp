#include "gadget.h"

void GadgetBase::setup() {
  delay(2000);
  activityBoard.setup();
  LOG("gadget setup done.");
}

void GadgetBase::loop() {
  scheduler.execute();
}
