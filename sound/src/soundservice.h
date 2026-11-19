#pragma once
#include <maintenace/maintenancecontroller.h>
#include <uart/uartcontroller.h>

#include "audioplayer.h"

class SoundService : public StreamCommandHandler {
 public:
  SoundService();

  void setup();
  void loop();

  void handleStreamCommand(StreamCommand cmd, const char* params) override;

  AudioPlayer audioPlayer;
  UartController uartController;
  StreamController consoleController;
  MaintenanceController maintenanceController;
};
