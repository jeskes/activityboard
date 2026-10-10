#include <rotaryDecoderSwitch.h>

#include "ablib.h"

#define POLLING_TIMEOUT 5

static rotaryDecoderSwitch decoder(ROTARY_ENCODER_MODULE_ID);

static void pollEncoderStatus();
static RotaryStatusHandler* statusHandler = nullptr;
static RotaryStatus lastStatus;
static Task encoderPollingTask(POLLING_TIMEOUT, TASK_FOREVER, pollEncoderStatus);

ActivityRotaryClient::ActivityRotaryClient(Scheduler& scheduler) {
  scheduler.addTask(encoderPollingTask);
}

void ActivityRotaryClient::setup(RotaryStatusHandler* handler) {
  LOG("ActivityRotaryClient: setup.");
  decoder.begin();
  decoder.readInitialState();
  setStatusHandler(handler);
  LOG("ActivityRotaryClient: successfully initialized.");
}

void ActivityRotaryClient::reset() {
  decoder.reset();
}

void ActivityRotaryClient::reset(uint8_t idx) {
  decoder.reset(idx);
}

void ActivityRotaryClient::setValue(uint8_t idx, uint32_t value) {
  decoder.setValue(idx, value);
}

RotaryStatus ActivityRotaryClient::getStatus() {
  return lastStatus;
}

void ActivityRotaryClient::setStatusHandler(RotaryStatusHandler* handler) {
  statusHandler = handler;
  lastStatus.clear();
  if (handler) {
    encoderPollingTask.enable();
  }
  else {
    encoderPollingTask.disable();
  }
}

static void pollEncoderStatus() {
  static unsigned long lastUpdate[2] = {0, 0};

  if (decoder.checkChange()) {
    decoder.update();

    RotaryStatus status;
    unsigned long now = millis();

    for (int idx = 0; idx < 2; idx++) {
      status[idx].value = decoder.getValue(idx);
      status[idx].pressed = decoder.isKeyPressed(idx);

      /* debounce */

      if (status[idx].value != lastStatus[idx].value) {
        int32_t delta = status[idx].value != lastStatus[idx].value;
        if ((now - lastUpdate[idx] < 60) && (delta > 1 || delta < -1)) {
          decoder.setValue(idx, lastStatus[idx].value); /* force to old value */
          status[idx].value = lastStatus[idx].value;
        }
        else {
          // LOG("value %d : %ld", idx, status[idx].value);
          lastUpdate[idx] = now;
        }
      }
    }
    if (statusHandler && memcmp(&lastStatus, &status, sizeof(RotaryStatus))) {
      bool cancelHandler = statusHandler->rotaryStatusChanged(status);
      if (cancelHandler) {
        statusHandler = nullptr;
        encoderPollingTask.disable();
      }
    }

    lastStatus = status;
  }
}

void pollEncoderStatusyy() {
  static int32_t lastValue = 0;
  static unsigned long lastMoveTime = 0;

  // 1. Schnell prüfen, ob der PCF8574 eine Änderung meldet
  if (decoder.checkChange()) {
    decoder.update();
    int32_t currentValue = decoder.getValue(0);

    // 2. Nur verarbeiten, wenn sich der Wert tatsächlich geändert hat
    if (currentValue != lastValue) {
      unsigned long now = millis();
      int32_t delta = currentValue - lastValue;

      // Filter gegen das Springen beim Richtungswechsel
      if ((now - lastMoveTime < 35) && (delta > 1 || delta < -1)) {
        decoder.setValue(0, lastValue);  // Auf alten Zustand zurückzwingen
      }
      else {
        Serial.print("Encoder-Wert: ");
        Serial.println(currentValue);

        lastValue = currentValue;
        lastMoveTime = now;
      }
    }
  }
}

RotaryEncoderStatus& RotaryStatus::operator[](uint8_t id) {
  return encoderStatus[id];
}

void RotaryStatus::clear() {
  memset(&encoderStatus, 0, sizeof(encoderStatus));
}
