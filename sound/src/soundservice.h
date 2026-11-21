#pragma once
#include <uart/uartcontroller.h>
#include <maintenace/maintenancecontroller.h>

#include "audioplayer.h"
#include "trackmanager.h"
#include "maintenancecontroller.h"

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
