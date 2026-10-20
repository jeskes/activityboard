#include "rotary.h"

#include "../display/display.h"

static void pollEncoderStatus();
static RotaryStatusHandler* statusHandler = nullptr;
static RotaryStatus lastStatus;
static Task encoderPollingTask(ROTARY_POLLING_TIMEOUT, TASK_FOREVER, pollEncoderStatus);

ActivityRotaryClient::ActivityRotaryClient(Scheduler& scheduler) {
  scheduler.addTask(encoderPollingTask);
}

void ActivityRotaryClient::setup() {
  pollEncoderStatus();
  LOG("ActivityRotaryClient: successfully setup: left=(%d,%ld), right=(%d,%ld)", (int)lastStatus[0].pressed, lastStatus[0].value, (int)lastStatus[1].pressed, lastStatus[1].value);
}

RotaryStatus ActivityRotaryClient::getStatus() {
  pollEncoderStatus();
  return lastStatus;
}

void ActivityRotaryClient::setStatusHandler(RotaryStatusHandler* handler, unsigned long pollingDelay, unsigned long pollingTimeout) {
  if (handler != statusHandler) {
    statusHandler = handler;
    if (statusHandler) {
      encoderPollingTask.enableDelayed(pollingDelay);
      encoderPollingTask.setInterval(pollingTimeout);
    }
    else {
      encoderPollingTask.disable();
    }
  }
}

void ActivityRotaryClient::pause() {
  encoderPollingTask.disable();
}

void ActivityRotaryClient::resume(unsigned long pollingDelay) {
  if (!encoderPollingTask.isEnabled() && statusHandler) {
    encoderPollingTask.enableDelayed(pollingDelay);
  }
}

bool ActivityRotaryClient::isPaused() {
  return !encoderPollingTask.isEnabled();
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
