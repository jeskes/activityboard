#include <modules/coreclient.h>
#include <modules/gadget.h>

#include "nfccontroller.h"

class MusicGadget : public GadgetBase, public CoreStatusHandler {
 public:
  MusicGadget();

  void setup();
  void loop();
  void handleCoreStatus(CoreStatus status) override;

 private:
  ActivityCoreClient core;
  NfcController nfc;
};
