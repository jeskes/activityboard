#include "storygadget.h"

#include "const.h"

StoryGadget::StoryGadget()
    : core(STORYGADGET_ID),
      nfc(core),
      definition(core),
      selectionController(definition, core, inputController),
      storyController(definition, core, inputController) {
}

void StoryGadget::setup() {
  pinMode(SELECTING_LED, OUTPUT);
  pinMode(READING_LED, OUTPUT);
  pinMode(MENU_LED, OUTPUT);

  GadgetBase::setup(READING_LED);
  SPI.begin();

  core.setup(CORE_STATUS_IRQ);
  nfc.setup();
  definition.setup();
  inputController.setup();
  selectionController.setup();
  storyController.setup();

  nfc.setHandler(this);

  core.init(STARTUP_IMAGE);
  core.playSound(STARTUP_TRACK);
  core.setStatusHandler(this);

  LOG("Story Gadget started.");
}

void StoryGadget::loop() {
  GadgetBase::loop();

  core.loop();
  nfc.loop();

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
      core.displayImage(STARTUP_IMAGE);
    }
    Input input = inputController.get();
    if (input == StdInput::START) {
      LOG("started");
      selectionController.run();
      resetPending = true;
    }
    else if (input >= StdInput::NUM_1 && (int)input <= definition.storiesCount) {
      LOG("story %d selected", input);
      resetPending = true;
      storyController.run(definition.getStoryId((int)input - 1));
    }
  }

  digitalWrite(SELECTING_LED, selectionController.isBusy() ? HIGH : LOW);
  digitalWrite(READING_LED, (storyController.isBusy() && !storyController.inMenu()) ? HIGH : LOW);
  digitalWrite(MENU_LED, (storyController.isBusy() && storyController.inMenu()) ? HIGH : LOW);
}

void StoryGadget::onNewRfidCard(RfidId rfid) {
  StoryId story = definition.getStoryIdByRfid(rfid);
  if (!story) {
    return;
  }

  if (selectionController.isBusy()) {
    selectionController.stop();
  }
  else if (!storyController.isBusy()) {
    storyController.stop();
  }

  LOG("rfid story %d selected", story);
  resetPending = true;
  storyController.run(story);
}

void StoryGadget::onRfidCardRemoved() {
  /* ignore card removal */
}

void StoryGadget::handleCoreStatus(CoreStatus coreStatus) {
}