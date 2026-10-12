
#include "ablib.h"

#define POLLING_TIMEOUT 5

static void pollEncoderStatus();
static RotaryStatusHandler* statusHandler = nullptr;
static RotaryStatus lastStatus;
static Task encoderPollingTask(POLLING_TIMEOUT, TASK_FOREVER, pollEncoderStatus);

ActivityRotaryClient::ActivityRotaryClient(Scheduler& scheduler) {
  scheduler.addTask(encoderPollingTask);
  lastStatus[ROTARY_ENCODER_LEFT].value = 0;
  lastStatus[ROTARY_ENCODER_LEFT].pressed = false;
  lastStatus[ROTARY_ENCODER_RIGHT].value = 0;
  lastStatus[ROTARY_ENCODER_RIGHT].pressed = false;
}

void ActivityRotaryClient::setup(RotaryStatusHandler* handler) {
  LOG("ActivityRotaryClient: setup.");
  setStatusHandler(handler);
  LOG("ActivityRotaryClient: successfully initialized.");
}

RotaryStatus ActivityRotaryClient::getStatus() {
  pollEncoderStatus();
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
  DisplayStatus status;
  PeriphericalClient::requestStatus(DISPLAY_MODULE_ID, &status, sizeof(DisplayStatus));
  if (statusHandler && memcmp(&lastStatus, &status.rotaryStatus, sizeof(RotaryStatus))) {
    bool cancelHandler = statusHandler->rotaryStatusChanged(status.rotaryStatus);
    if (cancelHandler) {
      statusHandler = nullptr;
      encoderPollingTask.disable();
    }
  }
  lastStatus = status.rotaryStatus;
}

/* some status helpers */

RotaryStatus::RotaryStatus() {
  memset(&encoderStatus, 0, sizeof(encoderStatus));
}

RotaryEncoderStatus& RotaryStatus::operator[](uint8_t id) {
  return encoderStatus[id];
}

void RotaryStatus::clear() {
  memset(&encoderStatus, 0, sizeof(encoderStatus));
}
