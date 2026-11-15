#include "musicgadget.h"

#include <SPI.h>

#include "const.h"

MusicGadget::MusicGadget()
    : core(MUSICGADGET_ID),
      nfc(core),
      sequential(-1) {
}

void MusicGadget::setup() {
  GadgetBase::setup(INDICATOR_PLAYING);

  SPI.begin();

  nfc.setup();
  core.setup(CORE_STATUS_IRQ);
  inputController.setup();

  core.init(STARTUP_IMAGE);
  core.playSound(STARTUP_TRACK);
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
  sequential = -1;
}

void MusicGadget::playSequential(int index) {
  sequential = index;
  LOG("play sequential track: category=%d, sequential=%d", category, sequential);
  core.playMusic(category, sequential);
  playing = true;
}

void MusicGadget::playRfidTrack(RfidId rfid) {
  sequential = -1;
  core.playMusic(rfid);
  playing = true;
}

void MusicGadget::handleCoreStatus(CoreStatus coreStatus) {
  playing = coreStatus.playerState == PlayerState::PLAYING;
  if (!playing && sequential >= 0) {
    next();
  }

  if (coreStatus.control.has(CoreControl::STOP_ALL)) {
    stopAll = true;
  }

  handleInput(coreStatus.webState.input);
}
