#include "storygadget.h"

#include "const.h"

StoryGadget::StoryGadget()
    : core(STORYGADGET_ID),
      nfc(RC522_SS, RC522_RST),
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
  if (!nfc.setup()) {
    activityControls.indicateError(READING_LED);
  }
  definition.setup();
  inputController.setup();
  selectionController.setup();
  storyController.setup();

  core.init(STARTUP_IMAGE);
  core.playTrack(STARTUP_TRACK);
  core.setStatusHandler(this);

  LOG("Story Gadget started.");
}

void StoryGadget::loop() {
  GadgetBase::loop();

  core.loop();
  RfidId rfid = nfc.loop();
  if (rfid) {
    onNewRfidCard(rfid);
  }

  if (selectionController.isBusy()) {
    StoryId selectedStory = selectionController.loop();
    if (selectedStory) {
      resetPending = true;
      startStory(selectedStory);
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
      StoryId storyId = definition.getStoryId((int)input - 1);
      startStory(storyId);
    }
  }

  digitalWrite(SELECTING_LED, selectionController.isBusy() ? HIGH : LOW);
  digitalWrite(READING_LED, (storyController.isBusy() && !storyController.inMenu()) ? HIGH : LOW);
  digitalWrite(MENU_LED, (storyController.isBusy() && storyController.inMenu()) ? HIGH : LOW);
}

void StoryGadget::onNewRfidCard(RfidId rfid) {
  StoryId storyId = definition.getStoryIdByRfid(rfid);
  if (!storyId) {
    return;
  }

  if (selectionController.isBusy()) {
    selectionController.stop();
  }
  else if (!storyController.isBusy()) {
    storyController.stop();
  }

  LOG("rfid story %d selected", storyId);
  startStory(storyId);
}

void StoryGadget::startStory(StoryId id) {
  resetPending = true;
  const uint8_t blinks[] = {SELECTING_LED, READING_LED, MENU_LED};
  activityControls.blink(blinks, 3, 3, 100);
  StoryDef* storyDef = definition.getStory(id);
  if (storyDef) {
    storyDef->assertLoaded(core);
    storyController.run(storyDef);
  }
}

void StoryGadget::handleCoreStatus(CoreStatus coreStatus) {
}