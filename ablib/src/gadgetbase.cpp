#include "ablib.h"

void GadgetBase::setup() {
  activityBoard.init();
  LOG("activity boad initialized");
}

void GadgetBase::loop() {
	scheduler.execute();
}
