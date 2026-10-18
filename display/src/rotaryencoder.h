#pragma once

#include <ESP32Encoder.h>
#include <rotary/rotary.h>

class RotaryEncoderWrapper {
 public:
  void setup();
  void reset();

  static void getStatus(RotaryStatus& status);
};
