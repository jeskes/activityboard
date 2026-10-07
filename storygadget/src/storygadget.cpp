#include "storygadget.h"

#include <Wire.h>
#include <ablib.h>

#include "const.h"

StoryGadget::StoryGadget()
    : sound(STORYGADGET_ID, this->scheduler),
      display(STORYGADGET_ID),
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

  LOG("Story Gadget started.");
}

void StoryGadget::loop() {
  GadgetBase::loop();

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
    }
  }

  digitalWrite(SELECTING_LED, selectionController.isBusy() ? HIGH : LOW);
  digitalWrite(READING_LED, storyController.isBusy() ? HIGH : LOW);
}
