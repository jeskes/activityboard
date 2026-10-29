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
    Input in = input.get();
    if (in == Input::SELECT) {
      LOG("selection started");
      selection.run();
      resetPending = true;
    }
    else if (in >= Input::NUM_1 && (int)in <= selection.getStoriesCount()) {
      LOG("story %d selected", in);
      resetPending = true;
      story.run(selection.getStory((int)in-1));
    }
  }

  digitalWrite(SELECTING_LED, selection.isBusy() ? HIGH : LOW);
  digitalWrite(READING_LED, story.isBusy() ? HIGH : LOW);
}
