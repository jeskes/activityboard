#pragma once

#include <modules/gadget.h>
#include <modules/coreclient.h>
#include "inputcontroller.h"

class DiceGadget : public GadgetBase, public CoreStatusHandler {
 public:
  DiceGadget();

  void setup();
  void loop();
  void handleCoreStatus(CoreStatus status) override;

 private:
  ActivityCoreClient core;
  InputController input;

  void stateMachine();
  void showIndicators();
  void throwDice();
  void reset();

  void handleIdleInput(Input in);
  void setGuess(int guess);
  void setJingle(int jingle);

  enum class Status : int {
    IDLE = 0,
    START_REQUESTED = 1,
    JINGLE_SOUND_PLAYING = 2,
    JINGLE_SOUND_ENDED = 3,
    NUMBER_SOUND_PLAYING = 4,
    NUMBER_SOUND_ENDED = 5,
    NUMBER_INDICATING = 6
  };

  Status status;

  int currentGuess;
  int currentNumber;
  int currentJingle;

  unsigned long startedTime;
  unsigned long indicatingTime;
  unsigned long displayResetPending;
  unsigned long guessIndicatingTime;

  void checkDisplayReset();
  void scheduleDisplayReset();
  void cancelDisplayReset();

  void playJingle();
  void playNumber(int number);
};
