#include "ablib.h"

void GadgetBase::setup() {
  activityBoard.setup();
  LOG("activity boad initialized");
}

void GadgetBase::loop() {
	scheduler.execute();
}
