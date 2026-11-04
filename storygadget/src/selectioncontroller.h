#pragma once

#include <ArduinoJson.h>
#include <modules/coreclient.h>

#include "const.h"
#include "definition.h"
#include "inputcontroller.h"

class SelectionController {
 public:
  SelectionController(Definition& definiton, ActivityCoreClient& core, InputController& input);

  void setup();
  void run();
  StoryId loop();
  bool isBusy();

 private:
  Definition& definition;
  ActivityCoreClient& core;
  InputController& input;

  int currentIndex = -1;
  unsigned long currentTimeout = 0;

  void initCurrentSelection();
  StoryId onStorySelected(int index);
};
