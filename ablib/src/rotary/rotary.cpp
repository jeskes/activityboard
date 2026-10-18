
#include "rotary.h"
#include "../display/display.h"

#define POLLING_TIMEOUT 10

static void pollEncoderStatus();
static RotaryStatusHandler* statusHandler = nullptr;
static RotaryStatus lastStatus;
static Task encoderPollingTask(POLLING_TIMEOUT, TASK_FOREVER, pollEncoderStatus);

ActivityRotaryClient::ActivityRotaryClient(Scheduler& scheduler) {
  scheduler.addTask(encoderPollingTask);
}

void ActivityRotaryClient::setup() {
  LOG("ActivityRotaryClient: setup.");
  pollEncoderStatus();
  LOG("ActivityRotaryClient: successfully initialized.");
}

RotaryStatus ActivityRotaryClient::getStatus() {
  pollEncoderStatus();
  return lastStatus;
}

void ActivityRotaryClient::setStatusHandler(RotaryStatusHandler* handler) {
  if (handler != statusHandler) {
    statusHandler = handler;
    if (statusHandler) {
      encoderPollingTask.enable();
    }
    else {
      encoderPollingTask.disable();
    }
  }
}

static void pollEncoderStatus() {
  // LOG("poll rotary status");
  DisplayStatus status;
  PeriphericalClient::requestStatus(DISPLAY_MODULE_ID, &status, sizeof(DisplayStatus));
  // LOG("received rotary status: (%d, %04d), (%d, %04d)", status.rotaryStatus[0].pressed, (int)status.rotaryStatus[0].value, status.rotaryStatus[1].pressed, (int)status.rotaryStatus[1].value);
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
