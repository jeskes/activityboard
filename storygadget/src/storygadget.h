#pragma once

#include <modules/modules.h>

#include "inputcontroller.h"
#include "selectionController.h"
#include "storycontroller.h"
#include "definition.h"

class StoryGadget : public GadgetBase {
 public:
  StoryGadget();

  void setup();
  void loop();

  ActivityCoreClient core;
  Definition definition;
  InputController input;
  SelectionController selection;
  StoryController story;

  bool resetPending = true;
};
