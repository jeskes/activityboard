#include <modules/coreclient.h>
#include <modules/gadget.h>

#include "inputcontroller.h"
#include "nfccontroller.h"

class MusicGadget : public GadgetBase, public CoreStatusHandler {
 public:
  MusicGadget();

  void setup();
  void loop();

  void handleInput(Input in);
  void handleCoreStatus(CoreStatus status) override;

 private:
  ActivityCoreClient core;
  NfcController nfc;
  InputController inputController;

  bool playing;
  bool wasPlaying;
  unsigned long notPlayingSince;

  uint8_t musicIndex;
};
