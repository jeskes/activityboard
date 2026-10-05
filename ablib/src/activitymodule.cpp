#include "ablib.h"

void GadgetBase::setup() {
  activityBoard.setup();
  LOG("GadgetBase: activity board initialized.");
}

void GadgetBase::loop() {
	scheduler.execute();
}
