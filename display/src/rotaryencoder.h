#pragma once

#include <ESP32Encoder.h>
#include <ablib.h>

class RotaryEncoderWrapper {
 public:
  void setup();
  void reset();

  static void getStatus(RotaryStatus& status);
};
