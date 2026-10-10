#ifndef _AB_DICE_GADGET_H
#define _AB_DICE_GADGET_H

#include <ablib.h>

const ClientId DICEGADGET_ID = 11;

/* languages: 00x english, 01x bavarian, 02x russian, 03x german */
#define LANGUAGES_COUNT 4

/* jingle sounds: 101 ... 110 */
#define JINGLES_COUNT 10
#define INITIAL_JINGLE 101

class DiceGadget : GadgetBase, SoundStatusHandler, RotaryStatusHandler {
 public:
  DiceGadget();

  void setup();
  void loop();
  bool playerStateChanged(PlayerState state);
  bool rotaryStatusChanged(RotaryStatus status);

 private:
  ActivitySoundClient sound;
  ActivityNumpadClient numpad;
  ActivityRotaryClient rotary;
  ActivityDisplayClient display;

  void stateMachine();
  void showIndicators();
  void throwDice();
  void reset();

  void setGuess(int guess);
  bool checkNumpad();
  bool checkStartButton();

  enum class Status : int {
    IDLE = 0,
    START_REQUESTED = 1,
    JINGLE_SOUND_PLAYING = 2,
    JINGLE_SOUND_ENDED = 3,
    NUMBER_SOUND_PLAYING = 4,
    NUMBER_SOUND_ENDED = 5,
    NUMBER_INDICATING = 6
  };

  Status status = Status::IDLE;

  int selectedJingle = INITIAL_JINGLE;

  unsigned long wireBlock = 0;
  bool pendingGuessDisplay = false;

  int currentGuess = 0;
  int currentNumber = 0;
  int currentLanguage = 0;

  unsigned long startTime = 0;
  unsigned long startTimeIndicator = 0;
};

#endif