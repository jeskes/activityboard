#pragma once

#include "../base/base.h"

class GadgetBase {
 public:
  Scheduler scheduler;
  virtual void setup(uint8_t startupLed);
  virtual void loop();
};
