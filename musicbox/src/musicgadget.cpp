#include "musicgadget.h"

#include <SPI.h>

#include "const.h"

MusicGadget::MusicGadget()
    : core(MUSICGADGET_ID),
      nfc(core) {
}

void MusicGadget::setup() {
  GadgetBase::setup(INDICATOR_PLAYING);

  SPI.begin();

  nfc.setup();
  core.setup(CORE_STATUS_IRQ);

  core.init(STARTUP_IMAGE);
  core.playSound(STARTUP_TRACK);
  core.setStatusHandler(this);

  pinMode(STOP_BUTTON, INPUT_PULLUP);

  LOG("Music Gadget started.");
}

void MusicGadget::loop() {
  GadgetBase::loop();
  nfc.loop();
  core.loop();

  if (playing && activityControls.buttonPressed(STOP_BUTTON)) {
    core.stopSound();
    playing = false;
    digitalWrite(INDICATOR_PLAYING, playing ? HIGH : LOW);
  }
}

void MusicGadget::handleCoreStatus(CoreStatus coreStatus) {
  playing = coreStatus.playerState == PlayerState::PLAYING;
  digitalWrite(INDICATOR_PLAYING, playing ? HIGH : LOW);
  if (!playing) {
    core.displayImage(STARTUP_IMAGE);
  }
}