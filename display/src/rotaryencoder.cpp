#include "rotaryencoder.h"

#include "const.h"

static ESP32Encoder encoders[2];

struct RotaryEncoderPins {
  uint8_t clk;
  uint8_t dt;
  uint8_t sw;
};

static void initPins(RotaryEncoderPins pins);

void RotaryEncoderWrapper::setup() {
  initPins({ROTARY_ENCODER_LEFT_CLK, ROTARY_ENCODER_LEFT_DT, ROTARY_ENCODER_LEFT_SW});
  initPins({ROTARY_ENCODER_RIGHT_CLK, ROTARY_ENCODER_RIGHT_DT, ROTARY_ENCODER_RIGHT_SW});

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
  status[ROTARY_ENCODER_RIGHT].pressed = digitalRead(ROTARY_ENCODER_RIGHT_SW) == LOW;
}

static void initPins(RotaryEncoderPins pins) {
  pinMode(pins.clk, INPUT);
  pinMode(pins.dt, INPUT);
  pinMode(pins.sw, INPUT_PULLUP);
}
