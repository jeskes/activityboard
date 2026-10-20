#include "rotaryencoder.h"

#include "const.h"

static ESP32Encoder encoders[2];

struct RotaryEncoderPins {
  uint8_t clk;
  uint8_t dt;
  uint8_t sw;
};

static void initPins(RotaryEncoderPins pins);

static ActivityButton leftButton(ROTARY_ENCODER_LEFT_SW);
static ActivityButton rightButton(ROTARY_ENCODER_RIGHT_SW);

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
  status.encoderStatus[0].value = (int32_t)encoders[ROTARY_ENCODER_LEFT].getCount();
  status.encoderStatus[1].value = (int32_t)encoders[ROTARY_ENCODER_RIGHT].getCount();

  status.encoderStatus[0].pressed = leftButton.pressed();
  status.encoderStatus[1].pressed = rightButton.pressed();

  LOG("retrieved rotary status: (%d, %ld), (%d, %ld)", (int)status.encoderStatus[0].pressed, status.encoderStatus[0].value, (int)status.encoderStatus[1].pressed, status.encoderStatus[1].value);
}

static void initPins(RotaryEncoderPins pins) {
  pinMode(pins.clk, INPUT);
  pinMode(pins.dt, INPUT);
  pinMode(pins.sw, INPUT_PULLUP);
}
