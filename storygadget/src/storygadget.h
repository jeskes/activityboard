#pragma once

#include <modules/modules.h>
#include <storage/storage.h>

#include "const.h"
#include "inputcontroller.h"
#include "selectionController.h"
#include "storycontroller.h"

const ClientId STORYGADGET_ID = 12;

class StoryGadget : public GadgetBase {
 public:
  StoryGadget();

  void setup();
  void loop();

  Storage storage;
  ActivityCoreClient core;
  
  InputController input;
  SelectionController selection;
  StoryController story;

  bool resetPending = true;
};
