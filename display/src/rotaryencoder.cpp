#include "rotaryencoder.h"

#include <Wire.h>

#include "const.h"

static ESP32Encoder encoders[2];

void RotaryEncoderWrapper::setup() {
  pinMode(ROTARY_ENCODER_LEFT_CLK, INPUT);
  pinMode(ROTARY_ENCODER_LEFT_DT, INPUT);
  pinMode(ROTARY_ENCODER_LEFT_SW, INPUT_PULLUP);

  pinMode(ROTARY_ENCODER_RIGHT_CLK, INPUT);
  pinMode(ROTARY_ENCODER_RIGHT_DT, INPUT);
  pinMode(ROTARY_ENCODER_RIGHT_SW, INPUT_PULLUP);

  ESP32Encoder::useInternalWeakPullResistors = puType::up;
  encoders[ROTARY_ENCODER_LEFT].attachHalfQuad(ROTARY_ENCODER_LEFT_DT, ROTARY_ENCODER_LEFT_CLK);
  encoders[ROTARY_ENCODER_RIGHT].attachHalfQuad(ROTARY_ENCODER_RIGHT_DT, ROTARY_ENCODER_RIGHT_CLK);
}

void RotaryEncoderWrapper::reset() {
  encoders[ROTARY_ENCODER_LEFT].setCount(0);
  encoders[ROTARY_ENCODER_RIGHT].setCount(0);
}

void RotaryEncoderWrapper::getStatus(RotaryStatus& status) {
  status[ROTARY_ENCODER_LEFT].value = encoders[ROTARY_ENCODER_LEFT].getCount();
  status[ROTARY_ENCODER_LEFT].pressed = digitalRead(ROTARY_ENCODER_LEFT_SW) == LOW;

  status[ROTARY_ENCODER_RIGHT].value = encoders[ROTARY_ENCODER_RIGHT].getCount();
  status[ROTARY_ENCODER_RIGHT].pressed = digitalRead(ROTARY_ENCODER_RIGHT_SW) == LOW;;

  // LOG("rotary status: (%d, %04ld), (%d, %04ld)", status[0].pressed, status[0].value, status[1].pressed, status[1].value);
}