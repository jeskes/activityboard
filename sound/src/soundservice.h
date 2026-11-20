#pragma once
#include <maintenace/maintenancecontroller.h>
#include <uart/uartcontroller.h>

#include "audioplayer.h"
#include "trackmanager.h"

class SoundService : public StreamCommandHandler {
 public:
  SoundService();

  void setup();
  void loop();

  void handleStreamCommand(StreamCommand cmd, const char* params) override;

  AudioPlayer audioPlayer;
  TrackManager trackManager;
  UartController uartController;
  StreamController consoleController;
  MaintenanceController maintenanceController;
};
