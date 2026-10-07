#pragma once

#include <ablib.h>

#include "const.h"
#include "storage.h"
#include "selectionController.h"
#include "storycontroller.h"

const ClientId STORYGADGET_ID = 12;

class StoryGadget : public GadgetBase {
 public:
  StoryGadget();

  void setup();
  void loop();

  Storage storage;
  ActivitySoundClient sound;
  ActivityNumpadClient numpad;
  ActivityDisplayClient display;

  SelectionController selectionController;
  StoryController storyController;

  bool resetPending = true;
};
