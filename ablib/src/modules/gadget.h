#pragma once

#include "../base/base.h"

class GadgetBase {
 public:
  Scheduler scheduler;
  virtual void setup();
  virtual void loop();
};
