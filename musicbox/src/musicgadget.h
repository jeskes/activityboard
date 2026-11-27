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
  void reset(bool playTrack);

  void start();
  void repeat();
  void next();
  void stop();

  uint8_t category;
  void onCategoryChanged(MusicCategory category);

  enum class Mode {
    INITIAL,
    SELECTION,
    SEQUENTIAL
  };
  Mode mode;
  bool paused;

  int sequential[CATEGORY_COUNT];
  
  void playTrack();
  void playRfidTrack(RfidId rfid);
};
