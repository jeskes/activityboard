#include <modules/coreclient.h>
#include <modules/gadget.h>
#include <nfc/nfccontroller.h>

#include "categoryswitch.h"
#include "inputcontroller.h"

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
  void resetDisplay();

  void start();
  void repeat();
  void next();
  void stop();

  uint8_t category;
  void onCategoryChanged(MusicCategory category);

  enum class Sequential {
    RESET,
    REPEAT,
    NEXT
  };
  void playSequential(Sequential index);
  int sequential[CATEGORY_COUNT];

  void playRfidTrack(RfidId rfid);
};
