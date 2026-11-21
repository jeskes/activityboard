#include "soundservice.h"

#include <SD.h>
#include <SPI.h>
#include <base/base.h>

#include "const.h"
#include "soundcom.h"

static MaintenanceController::Configuration maintenanceConfiguration = {.hostname = "sound", .wifiBandwidth = WIFI_BW_HT20, .wifiPower = WIFI_POWER_11dBm};

SoundService::SoundService()
    : audioPlayer(SD),
      trackManager(SD),
      uartController(Serial1),
      consoleController(Serial),
      maintenanceController(maintenanceConfiguration) {
}

void SoundService::setup() {
  WiFi.mode(WIFI_OFF);
  WiFi.setSleep(false);
  btStop();

  Serial.begin(SERIAL_BAUD);
  Serial.setTxTimeoutMs(0);
  LOG("starting");

  audioPlayer.setup();
  uartController.setup(UART_RX, UART_TX);
  uartController.setHandler(this);
  consoleController.setHandler(this);

  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS, SPI, 20000000)) {
    uartController.send(SoundCom::ERROR, "sd initialization failed");
    LOG("sd card initialization failed");
  }
  else {
    uartController.send(SoundCom::INFO, "sd module ready");
  }

  delay(100);
  trackManager.setup();

  LOG("sound service initialized");
}

void SoundService::loop() {
  if (maintenanceController.isRunning()) {
    maintenanceController.loop();
  }
  else {
    audioPlayer.loop();
  }
  if( !maintenanceController.isBusy() ) {
	  uartController.loop();
	  consoleController.loop();
  }
}

void SoundService::handleStreamCommand(StreamCommand cmd, const char* params) {
  LOG("command: cmd=%s, params=%s", cmd, params);
  if (!strcmp(cmd, SoundCom::PLAY)) {
    const char* path = trackManager.mapTrackPath(params);
    if (audioPlayer.play(path)) {
      uartController.send(SoundCom::PLAYING, path);
    }
    else {
      uartController.send(SoundCom::ERROR, "cannot play track");
    }
  }
  else if (!strcmp(cmd, SoundCom::STOP)) {
    audioPlayer.stop();
    uartController.send(SoundCom::INFO, "stopped");
  }
  else if (!strcmp(cmd, SoundCom::VOLUME)) {
    audioPlayer.setVolume(atoi(params));
    uartController.send(SoundCom::INFO, "volume set");
  }
  else if (!strcmp(cmd, SoundCom::START_MAINTENANCE)) {
    maintenanceController.start();
    uartController.send(SoundCom::INFO, "maintenance started");
  }
  else if (!strcmp(cmd, SoundCom::STOP_MAINTENANCE)) {
    maintenanceController.stop();
	trackManager.setup();
    uartController.send(SoundCom::INFO, "stopped maintenance");
  }
}
