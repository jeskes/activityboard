#include <modules/coreclient.h>
#include <modules/gadget.h>

#include "categoryswitch.h"
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
  CategorySwitch categorySwitch;

  bool playing;
  bool wasPlaying;
  unsigned long notPlayingSince;
  bool stopAll;

  void start();
  void repeat();
  void next();
  void stop();

  int sequential;
  uint8_t category;
  void playSequential(int index);
  void playRfidTrack(RfidId rfid);
};
