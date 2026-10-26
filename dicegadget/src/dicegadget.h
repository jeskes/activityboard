#pragma once

#include <core/core.h>
#include <modules/modules.h>
#include <numpad/numpad.h>

#include "const.h"

class DiceGadget : GadgetBase, CoreStatusHandler {
 public:
  DiceGadget();

  void setup();
  void loop();
  bool playerStateChanged(PlayerState state);

 private:
  ActivityCoreClient core;
  ActivityNumpadClient numpad;

  void stateMachine();
  void showIndicators();
  void throwDice();
  void reset();

  void setGuess(int guess, bool updateDisplay);
  void setJingle(int jingle);
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
  
  int currentGuess = 0;
  int currentNumber = 0;
  int currentLanguage = 0;
  int selectedJingle = INITIAL_JINGLE;

  unsigned long startTime = 0;
  unsigned long startTimeIndicator = 0;
  unsigned long resetDisplayTime = 0;
};
