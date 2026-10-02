#ifndef _AB_DICE_GADGET_H
#define _AB_DICE_GADGET_H

#include <ablib.h>

#define DICEGADGET_ID 11

class DiceGadget : public GadgetBase, public SoundStatusHandler {
 public:
  DiceGadget();

  void setup();
  void loop();
  bool playerStateChanged(PlayerState state);

 private:
  ActivitySoundClient sound;
  ActivityNumpadClient numpad;
  ActivityDisplayClient display;

  void stateMachine();
  void showIndicators();
  void reset();

  enum class Status : int {
    IDLE = 0,
    JINGLE_SOUND_PLAYING = 1,
    JINGLE_SOUND_ENDED = 2,
    NUMBER_SOUND_PLAYING = 3,
    NUMBER_SOUND_ENDED = 4,
    NUMBER_INDICATING = 5
  };

  Status status = Status::IDLE;

  int currentGuess = 0;
  int currentNumber = 0;
  int currentLanguage = 0;
  unsigned long startTime = 0;
  unsigned long startTimeIndicator = 0;
};

#endif