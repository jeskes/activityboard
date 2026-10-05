#include "storygadget.h"

#include <Wire.h>
#include <ablib.h>

#include "const.h"

const uint8_t buttons[] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};
const size_t countButtons = sizeof(buttons) / sizeof(uint8_t);

const uint8_t indicators[] = {LED_1, LED_2, LED_3, LED_4};
const size_t countIndicators = sizeof(indicators) / sizeof(uint8_t);

StoryGadget::StoryGadget()
    : sound(STORYGADGET_ID, this->scheduler),
      display(STORYGADGET_ID),
      controller(storage, sound, numpad, display) {
}

void StoryGadget::setup() {
  GadgetBase::setup();

  numpad.setup();
  storage.setup();

  for (uint8_t idx = 0; idx < countButtons; idx++) {
    pinMode(buttons[idx], INPUT_PULLUP);
  }
  for (uint8_t idx = 0; idx < countIndicators; idx++) {
    pinMode(indicators[idx], OUTPUT);
  }

  delay(10000);
  LOG("sending bitmap");
  display.displayBitmap(BITMAP_STARTUP);

  loadStories();

  activityControls.blink(indicators, countIndicators, 2, 200);

  LOG("Story Gadget started.");
}

void StoryGadget::loop() {
  GadgetBase::loop();

  controller.loop();

  if (!controller.isBusy()) {
    int idx = numpad.getChar() - (int)'1';
    if (idx >= 0 && idx < MAX_STORIES && stories[idx]) {
      LOG("story %d selected at numpad.", idx + 1);
      controller.run(stories[idx]);
    }
  }

  if (!controller.isBusy()) {
    for (uint8_t idx = 0; idx < countButtons; idx++) {
      if (activityControls.buttonPressed(buttons[idx])) {
        if (idx < MAX_STORIES && stories[idx]) {
          LOG("story %d selected at buttons.", idx + 1);
          controller.run(stories[idx]);
        }
      }
    }
  }

  int activeIndex = -1;
  for (uint8_t idx = 0; idx < MAX_STORIES; idx++) {
    if (stories[idx] == controller.activeStory) {
      activeIndex = idx;
      break;
    }
  }
  for (uint8_t idx = 0; idx < countIndicators; idx++) {
    pinMode(indicators[idx], idx == activeIndex ? HIGH : LOW);
  }
}

void StoryGadget::loadStories() {
  if (!storage.loadJson("/stories.json", storiesDocument)) {
    LOG("cannot load stories");
    return;
  }

  int idx = 0;
  JsonObject entries = storiesDocument.as<JsonObject>();
  for (JsonPair entry : entries) {
    if (idx < MAX_STORIES) {
      stories[idx] = entry.key().c_str();
      titles[idx] = entry.value().as<const char*>();
      idx++;
    }
    else {
      break;
    }
  }

  for (int idx = 0; idx < MAX_STORIES; idx++) {
    LOG("entry: id=%s, title=%s", stories[idx], titles[idx] ? titles[idx] : "undefined");
  }
}
