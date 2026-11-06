#pragma once

#include <button/button.h>
#include <rotaryDecoderSwitch.h>

class RotaryExpansion;

class RotaryAdapter {
 public:
  RotaryAdapter(long initialValue, long minValue, long maxValue, long step = 1, unsigned long pressedTimeout = BUTTON_PRESSED_TIMEOUT);

  void onPressed(std::function<void()> handler);
  void onValueChanged(std::function<void(long value)> handler);

 private:
  long step;
  long minValue;
  long maxValue;
  long lastValue;
  int32_t lastClicks;
  
  unsigned long pressedTimeout;
  std::function<void()> pressedHandler;
  std::function<void(long value)> valueChangedHandler;
  void onInterrupt(RotaryExpansion& rotary, int index);

  friend class RotaryExpansion;
};

class RotaryExpansion {
 public:
  RotaryExpansion();

  void setup();
  void loop();

  void setAdapterLeft(RotaryAdapter& adapter);
  void setAdapterRight(RotaryAdapter& adapter);

 private:
  rotaryDecoderSwitch decoder;

  RotaryAdapter* adapterLeft;
  RotaryAdapter* adapterRight;

  friend class RotaryAdapter;
};
