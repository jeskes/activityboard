#include "storygadget.h"

StoryGadget::StoryGadget()
    : core(STORYGADGET_ID, scheduler),
      selection(storage, core, input),
      story(storage, core, input) {
}

void StoryGadget::setup() {
  pinMode(SELECTING_LED, OUTPUT);
  pinMode(READING_LED, OUTPUT);

  const uint8_t blinks[] = {SELECTING_LED, READING_LED};
  activityControls.blink(blinks, 2, 5, 200);

  GadgetBase::setup();

  core.setup();
  storage.setup();
  input.setup();
  selection.setup();
  story.setup();

  core.init(BITMAP_STARTUP);

  LOG("Story Gadget started.");
}

void StoryGadget::loop() {
  GadgetBase::loop();

  if (selection.isBusy()) {
    StoryId selectedStory = selection.loop();
    if (selectedStory) {
      resetPending = true;
      story.run(selectedStory);
    }
  }
  else if (story.isBusy()) {
    story.loop();
  }
  else {
    if (resetPending) {
      resetPending = false;
      core.displayBitmap(BITMAP_STARTUP);
    }
    if (input.selectPressed()) {
      LOG("selection started");
      selection.run();
      resetPending = true;
    }
    else {
      int idx = input.numberPressed() - 1;
      if (idx >= 0 && idx < selection.getStoriesCount()) {
        LOG("story %d selected", idx + 1);
        resetPending = true;
        story.run(selection.getStory(idx));
      }
    }
  }

  digitalWrite(SELECTING_LED, selection.isBusy() ? HIGH : LOW);
  digitalWrite(READING_LED, story.isBusy() ? HIGH : LOW);
}
