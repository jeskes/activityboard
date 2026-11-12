#pragma once

#include <modules/coreclient.h>

#include "const.h"
#include "definition.h"
#include "inputcontroller.h"

class SelectionController {
 public:
  SelectionController(Definition& definiton, ActivityCoreClient& core, InputController& input);

  void setup();
  StoryId loop();

  void run();
  void stop();
  bool isBusy();

 private:
  Definition& definition;
  ActivityCoreClient& core;
  InputController& inputController;

  int currentIndex = -1;
  unsigned long currentTimeout = 0;

  void initCurrentSelection();
  StoryId onStorySelected(int index);
};
