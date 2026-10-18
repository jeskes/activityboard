#include "storygadget.h"

StoryGadget::StoryGadget()
    : sound(STORYGADGET_ID, scheduler),
      display(STORYGADGET_ID, scheduler),
      selectionController(storage, sound, numpad, display),
      storyController(storage, sound, numpad, display) {
}

void StoryGadget::setup() {
  GadgetBase::setup();

  numpad.setup();
  storage.setup();
  selectionController.setup();

  pinMode(START_BUTTON, INPUT_PULLUP);
  pinMode(SELECT_BUTTON, INPUT_PULLUP);
  pinMode(SELECTING_LED, OUTPUT);
  pinMode(READING_LED, OUTPUT);

  const uint8_t blinks[] = {SELECTING_LED, READING_LED};
  activityControls.blink(blinks, 2, 2, 200);

  display.init(BITMAP_STARTUP);

  LOG("Story Gadget started.");
}

static unsigned long heartbeat = 0;

void StoryGadget::loop() {
  GadgetBase::loop();

  if (millis() > heartbeat) {
    LOG("heartbeat: selection-busy=%d, story-busy=%d, storage=%d, numpad=%d", selectionController.isBusy(), storyController.isBusy(), storage.sd.cardType(), numpad.available());
    heartbeat = millis() + 2000;
  }

  if (selectionController.isBusy()) {
    StoryId selectedStory = selectionController.loop();
    if (selectedStory) {
      resetPending = true;
      storyController.run(selectedStory);
    }
  }
  else if (storyController.isBusy()) {
    storyController.loop();
  }
  else {
    if (resetPending) {
      resetPending = false;
      display.displayBitmap(BITMAP_STARTUP);
    }
    if (activityControls.buttonPressed(START_BUTTON)) {
      LOG("selection started using start button.");
      selectionController.run();
      resetPending = true;
    }
    else {
      int idx = numpad.getChar() - (int)'1';
      if (idx >= 0 && idx < selectionController.getStoriesCount()) {
        LOG("story %d directly selected at numpad.", idx + 1);
        resetPending = true;
        storyController.run(selectionController.getStory(idx));
      }
      if (idx == -1) {
        LOG("selection started at numpad.");
        selectionController.run();
        resetPending = true;
      }
    }
  }

  digitalWrite(SELECTING_LED, selectionController.isBusy() ? HIGH : LOW);
  digitalWrite(READING_LED, storyController.isBusy() ? HIGH : LOW);
}
