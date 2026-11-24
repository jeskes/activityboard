#pragma once

#include <modules/gadget.h>
#include <nfc/nfccontroller.h>

#include "definition.h"
#include "inputcontroller.h"
#include "selectionController.h"
#include "storycontroller.h"

class StoryGadget : public GadgetBase, public CoreStatusHandler {
 public:
  StoryGadget();

  void setup();
  void loop();

  void handleCoreStatus(CoreStatus coreStatus);
  void onNewRfidCard(RfidId rfid);

 private:
  ActivityCoreClient core;
  NfcController nfc;
  Definition definition;

  InputController inputController;
  SelectionController selectionController;
  StoryController storyController;

  bool resetPending = true;
};
