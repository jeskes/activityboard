#pragma once

#include <modules/modules.h>
#include <storage/storage.h>

#include "const.h"
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
