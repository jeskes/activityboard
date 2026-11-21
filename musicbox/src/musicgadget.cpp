#include "musicgadget.h"

#include <SPI.h>

#include "const.h"

MusicGadget::MusicGadget()
    : core(MUSICGADGET_ID),
      nfc(core),
      sequential(NO_SEQUENTIAL) {
}

void MusicGadget::setup() {
  GadgetBase::setup(INDICATOR_PLAYING);

  SPI.begin();

  nfc.setup();
  core.setup(CORE_STATUS_IRQ);
  categorySwitch.setup();
  inputController.setup();

  core.init(STARTUP_IMAGE);
  core.playTrack(STARTUP_TRACK);
  core.setStatusHandler(this);

  LOG("Music Gadget started.");
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
  if (category != MUSIC_CATEGORY_UNDEF) {
    onCategoryChanged(category);
  }

  if (!playing) {
    if (wasPlaying) {
      notPlayingSince = millis();
    }
    else if (notPlayingSince && millis() > notPlayingSince + NOTPLAYING_RESET_TIMEOUT) {
      core.displayImage(STARTUP_IMAGE);
      notPlayingSince = 0;
    }
  }
  wasPlaying = playing;

  digitalWrite(INDICATOR_PLAYING, playing ? HIGH : LOW);

  if (stopAll) {
    LOG("stop all");
    stopAll = false;
    stop();
    core.displayImage(STARTUP_IMAGE);
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
      if (input > 0 && input <= CATEGORY_SWITCH_COUNT) {
        category = input - 1;
        playSequential(0);
      }
	  break;
  }
}

void MusicGadget::start() {
  LOG("start");
  category = categorySwitch.get();
  playSequential(0);
}

void MusicGadget::repeat() {
  LOG("repeat");
  playSequential(sequential);
}

void MusicGadget::next() {
  LOG("next");
  playSequential(sequential + 1);
}

void MusicGadget::stop() {
  LOG("stop");
  core.stopSound();
  playing = false;
  sequential = NO_SEQUENTIAL;
}

void MusicGadget::playSequential(int index) {
  if (category != MUSIC_CATEGORY_UNDEF) {
    sequential = index;
    LOG("play sequential track: category=%d, sequential=%d", category, sequential);
    char path[ID_BUFFER_SIZE];
    sprintf(path, "/%02d/#%d", MUSIC_CATEGORY_FOLDERS[category], sequential);
    core.playTrack(path, SoundControl(SoundControl::DISPLAY_INFO));
    playing = true;
  }
}

void MusicGadget::playRfidTrack(RfidId rfid) {
  sequential = NO_SEQUENTIAL;
  char path[ID_BUFFER_SIZE];
  sprintf(path, "@%s", rfid);
  core.playTrack(path, SoundControl(SoundControl::DISPLAY_INFO));
  playing = true;
}

void MusicGadget::onCategoryChanged(MusicCategory category) {
  LOG("category changed: category=%d", category);
  char image[11];
  sprintf(image, "catagory-%01d", category);
  core.displayImage(image);
  core.stopSound();
  sequential = NO_SEQUENTIAL;
}

void MusicGadget::handleCoreStatus(CoreStatus coreStatus) {
  playing = coreStatus.playerState == PlayerState::PLAYING;
  if (!playing && sequential != NO_SEQUENTIAL) {
    next();
  }

  if (coreStatus.control.has(CoreControl::STOP_ALL)) {
    stopAll = true;
  }

  handleInput(coreStatus.webState.input);
}
