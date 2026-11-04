#pragma once

#include <modules/gadget.h>
#include <storage/storage.h>

#include "const.h"
#include "definition.h"
#include "inputcontroller.h"
#include "selectionController.h"
#include "storycontroller.h"

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
