#include "musicgadget.h"

#include <SPI.h>

#include "const.h"

MusicGadget::MusicGadget()
    : core(MUSICGADGET_ID),
      nfc(core) {
}

void MusicGadget::setup() {
  GadgetBase::setup();

  SPI.begin();

  nfc.setup();
  core.setup(CORE_STATUS_IRQ);

  core.init(STARTUP_IMAGE);
  core.setStatusHandler(this);

  pinMode(INDICATOR_PLAYING, OUTPUT);
  activityControls.blink(INDICATOR_PLAYING, 5, 200);

  LOG("Music Gadget started.");
}

void MusicGadget::loop() {
  GadgetBase::loop();
  nfc.loop();
  core.loop();
}

void MusicGadget::handleCoreStatus(CoreStatus coreStatus) {
  bool playing = coreStatus.playerState == PlayerState::PLAYING;
  digitalWrite(INDICATOR_PLAYING, playing ? HIGH : LOW);
}