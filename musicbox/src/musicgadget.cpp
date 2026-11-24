#include "musicgadget.h"

#include <SPI.h>

#include "const.h"

MusicGadget::MusicGadget()
    : core(MUSICGADGET_ID),
      nfc(RC522_SS, RC522_RST),
      sequential{0} {
}

void MusicGadget::setup() {
  GadgetBase::setup(INDICATOR_PLAYING);

  SPI.begin();

  if (!nfc.setup()) {
    activityControls.indicateError(INDICATOR_PLAYING);
  }

  core.setup(CORE_STATUS_IRQ);
  categorySwitch.setup();
  inputController.setup();

  core.setStatusHandler(this);

  category = categorySwitch.get();
  resetDisplay();

  LOG("Music Gadget started: category=%d", category);
}

void MusicGadget::loop() {
  GadgetBase::loop();
  core.loop();

  RfidId rfid = nfc.loop();
  if (rfid) {
    playRfidTrack(rfid);
  }
  else {
    Input input = inputController.get();
    handleInput(input);
  }

  MusicCategory category = categorySwitch.loop();
  if (category != CATEGORY_UNDEF) {
    onCategoryChanged(category);
  }

  if (!playing) {
    if (wasPlaying) {
      notPlayingSince = millis();
    }
    else if (notPlayingSince && millis() > notPlayingSince + NOTPLAYING_RESET_TIMEOUT) {
      resetDisplay();
      notPlayingSince = 0;
    }
  }
  wasPlaying = playing;

  digitalWrite(INDICATOR_PLAYING, playing ? HIGH : LOW);

  if (stopAll) {
    LOG("stop all");
    stopAll = false;
    stop();
  }
}

void MusicGadget::handleInput(Input input) {
  switch (input) {
    case StdInput::START:
      start();
      break;
    case StdInput::REPEAT:
      repeat();
      break;
    case StdInput::NEXT:
      next();
      break;
    case StdInput::STOP:
      stop();
      break;
    default:
      if (input > 0 && input <= 10) {
		sequential[category]= input-1;
        playSequential(Sequential::REPEAT);
      }
      break;
  }
}

void MusicGadget::resetDisplay() {
  char image[11];
  sprintf(image, "category-%01d", category);
  core.displayImage(image);
}

void MusicGadget::start() {
  LOG("start");
  playSequential(Sequential::RESET);
}

void MusicGadget::repeat() {
  LOG("repeat");
  playSequential(Sequential::REPEAT);
}

void MusicGadget::next() {
  LOG("next");
  playSequential(Sequential::NEXT);
}

void MusicGadget::stop() {
  LOG("stop");
  core.stopSound();
  playing = false;
  resetDisplay();
}

void MusicGadget::playSequential(Sequential index) {
  switch (index) {
    case Sequential::RESET:
      sequential[category] = 0;
      break;
    case Sequential::REPEAT:
      break;
    case Sequential::NEXT:
      sequential[category]++;
      break;
  }

  LOG("play sequential track: category=%d, sequential=%d", category, sequential[category]);
  char path[ID_BUFFER_SIZE];
  sprintf(path, "/%02d/#%d", CATEGORY_FOLDERS[category], sequential[category]);
  core.playTrack(path, SoundControl(SoundControl::DISPLAY_INFO));
  playing = true;
}

void MusicGadget::playRfidTrack(RfidId rfid) {
  char path[ID_BUFFER_SIZE];
  sprintf(path, "@%s", rfid);
  core.playTrack(path, SoundControl(SoundControl::DISPLAY_INFO));
  playing = true;
}

void MusicGadget::onCategoryChanged(MusicCategory category) {
  LOG("category changed: category=%d", category);
  this->category = category;
  resetDisplay();
  char alias[ID_BUFFER_SIZE];
  sprintf(alias, "@category-%01d", category);
  core.playTrack(alias);
}

void MusicGadget::handleCoreStatus(CoreStatus coreStatus) {
  playing = coreStatus.playerState == PlayerState::PLAYING;

  if (coreStatus.control.has(CoreControl::STOP_ALL)) {
    stopAll = true;
  }

  handleInput(coreStatus.webState.input);
}
