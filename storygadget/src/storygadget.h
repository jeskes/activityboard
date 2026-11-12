#pragma once

#include <modules/gadget.h>

#include "definition.h"
#include "inputcontroller.h"
#include "nfccontroller.h"
#include "selectionController.h"
#include "storycontroller.h"

class StoryGadget : public GadgetBase, public NfcEventHandler, public CoreStatusHandler {
 public:
  StoryGadget();

  void setup();
  void loop();

  void handleCoreStatus(CoreStatus coreStatus);
  void onNewRfidCard(RfidId rfid) override;
  void onRfidCardRemoved() override;

 private:
  ActivityCoreClient core;
  NfcController nfc;
  Definition definition;

  InputController inputController;
  SelectionController selectionController;
  StoryController storyController;

  bool resetPending = true;
};
