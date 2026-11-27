#include "musicgadget.h"

#include <SPI.h>

#include "const.h"

MusicGadget::MusicGadget()
    : core(MUSICGADGET_ID),
      nfc(RC522_RST),
      mode(Mode::INITIAL),
      paused(false),
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
  LOG("got category: category=%d", category);
  reset(true);

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
      reset(false);
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
        sequential[category] = input - 1;
        playTrack();
      }
      break;
  }
}

void MusicGadget::reset(bool playTrack) {
  paused = false;
  notPlayingSince = 0;
  mode = Mode::INITIAL;

  char buffer[ID_BUFFER_SIZE];
  sprintf(buffer, "@category-image-%01d", category);
  core.displayImage(buffer);

  if (playTrack) {
    sprintf(buffer, "@category-track-%01d", category);
    core.playTrack(buffer);
  }
}

void MusicGadget::start() {
  LOG("start");
  switch (mode) {
    case Mode::INITIAL:
      mode = Mode::SELECTION;
      sequential[category] = 0;
      break;
    case Mode::SELECTION:
      mode = Mode::SEQUENTIAL;
      break;
    case Mode::SEQUENTIAL:
      if (paused) {
        core.resumeTrack();
      }
      else {
        core.pauseTrack();
      }
      paused = !paused;
      return;
  }
  playTrack();
}

void MusicGadget::repeat() {
  LOG("repeat");
  if (mode != Mode::INITIAL) {
    playTrack();
  }
}

void MusicGadget::next() {
  LOG("next");
  if (mode != Mode::INITIAL) {
    sequential[category]++;
    mode = Mode::SELECTION;
    playTrack();
  }
}

void MusicGadget::stop() {
  LOG("stop");
  core.stopSound();
  playing = false;
  reset(false);
}

void MusicGadget::playTrack() {
  paused = false;
  char folder[10];
  sprintf(folder, "%02d", CATEGORY_FOLDERS[category]);
  if (mode == Mode::SELECTION) {
    strcat(folder, "/teaser");
  }

  LOG("play track: category=%d, sequential=%d, folder=%s", category, sequential[category], folder);
  char path[ID_BUFFER_SIZE];
  sprintf(path, "/%s/#%d", folder, sequential[category]);

  SoundControl soundControl(SoundControl::DISPLAY_INFO);
  core.playTrack(path, soundControl);

  playing = true;
}

void MusicGadget::playRfidTrack(RfidId rfid) {
  char path[ID_BUFFER_SIZE];
  sprintf(path, "@rfid-track-%s", rfid);
  core.playTrack(path, SoundControl(SoundControl::DISPLAY_INFO));
  playing = true;
}

void MusicGadget::onCategoryChanged(MusicCategory category) {
  LOG("category changed: category=%d", category);
  this->category = category;
  reset(true);
}

void MusicGadget::handleCoreStatus(CoreStatus coreStatus) {
  playing = coreStatus.playerState == PlayerState::PLAYING;

  if (coreStatus.control.has(CoreControl::STOP_ALL)) {
    stopAll = true;
  }

  handleInput(coreStatus.webState.input);
}
