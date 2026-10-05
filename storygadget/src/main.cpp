#include <TaskScheduler.h>
#include "storygadget.h"

StoryGadget storyGadget;

void setup() {
  storyGadget.setup();
  storyGadget.storage.diagnostics();
}

void loop() {
  storyGadget.loop();
}