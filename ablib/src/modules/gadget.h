#pragma once
#include <TaskSchedulerDeclarations.h>

#include "../base/base.h"

class GadgetBase {
 public:
  Scheduler scheduler;
  virtual void setup(uint8_t led);
  virtual void loop();
};
