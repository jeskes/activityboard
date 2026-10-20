#pragma once

#include "../base/base.h"

/* note that rotary encoders are hosted by the display module */

#define ROTARY_ENCODER_LEFT 0
#define ROTARY_ENCODER_RIGHT 1

#define ROTARY_POLLING_DELAY 200
#define ROTARY_POLLING_TIMEOUT 50

struct __attribute__((packed)) RotaryEncoderStatus {
  int32_t value;
  bool pressed;
};

class __attribute__((packed)) RotaryStatus {
 public:
  RotaryStatus();
  void clear();
  RotaryEncoderStatus& operator[](uint8_t idx);
  RotaryEncoderStatus encoderStatus[2];
};

class RotaryStatusHandler {
 public:
  virtual bool rotaryStatusChanged(RotaryStatus status) = 0;
};

class ActivityRotaryClient {
 public:
  ActivityRotaryClient(Scheduler& scheduler);
  void setup();

  RotaryStatus getStatus();
  void setStatusHandler(RotaryStatusHandler* handler, unsigned long pollingDelay = ROTARY_POLLING_DELAY, unsigned long pollingTimeout = ROTARY_POLLING_TIMEOUT);

  void pause();
  bool isPaused();
  void resume(unsigned long pollingDelay = ROTARY_POLLING_DELAY);
};
