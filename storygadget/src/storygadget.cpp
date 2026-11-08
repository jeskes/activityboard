#include "storygadget.h"

StoryGadget::StoryGadget()
    : core(STORYGADGET_ID, scheduler),
      definition(core),
      selection(definition, core, input),
      story(definition, core, input) {
}

void StoryGadget::setup() {
  pinMode(SELECTING_LED, OUTPUT);
  pinMode(READING_LED, OUTPUT);
  pinMode(MENU_LED, OUTPUT);

  const uint8_t blinks[] = {SELECTING_LED, READING_LED, MENU_LED};
  activityControls.blink(blinks, 3, 5, 200);

  GadgetBase::setup();

  core.setup();
  definition.setup();
  input.setup();
  selection.setup();
  story.setup();

  core.init(STARTUP_BITMAP);
  core.playSound(STARTUP_TRACK);

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
      core.displayBitmap(STARTUP_BITMAP);
    }
    Input in = input.get();
    if (in == StdInput::START) {
      LOG("started");
      selection.run();
      resetPending = true;
    }
    else if (in >= StdInput::NUM_1 && (int)in <= definition.storiesCount) {
      LOG("story %d selected", in);
      resetPending = true;
      story.run(definition.getStoryId((int)in - 1));
    }
  }

  digitalWrite(SELECTING_LED, selection.isBusy() ? HIGH : LOW);
  digitalWrite(READING_LED, (story.isBusy() && !story.inMenu()) ? HIGH : LOW);
  digitalWrite(MENU_LED, (story.isBusy() && story.inMenu()) ? HIGH : LOW);
}
