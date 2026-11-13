#include "musicgadget.h"

#include <SPI.h>

#include "const.h"

MusicGadget::MusicGadget()
    : core(MUSICGADGET_ID),
      nfc(core),
      musicIndex(0xff) {
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
  nfc.loop();
  core.loop();

  Input input = inputController.get();
  switch (input) {
    case StdInput::START:
      if (!playing) {
        musicIndex++;
        core.playMusic(musicIndex);
        playing = true;
      }
      break;
    case StdInput::REPEAT:
      core.playMusic(musicIndex);
      break;
    case StdInput::NEXT:
      musicIndex++;
      core.playMusic(musicIndex);
      break;
    case StdInput::STOP:
      core.stopSound();
      playing = false;
      break;
  }

  if (!playing) {
    if (!notPlayingSince) {
      notPlayingSince = millis();
    }
    else if (millis() > notPlayingSince + NOTPLAYING_RESET_TIMEOUT) {
      notPlayingSince = 0;
      core.displayImage(STARTUP_IMAGE);
    }
  }
  else {
	notPlayingSince = 0;
  }

  digitalWrite(INDICATOR_PLAYING, playing ? HIGH : LOW);
}

void MusicGadget::handleCoreStatus(CoreStatus coreStatus) {
  playing = coreStatus.playerState == PlayerState::PLAYING;
}
