#include "core.h"

#include <Arduino.h>
#include <Wire.h>

#define POLLING_TIMEOUT 500

static void pollCoreStatus();
static CoreStatusHandler* statusHandler = nullptr;
static PlayerState lastPlayerState = PlayerState::UNKNOWN;
static Task statusPollingTask(POLLING_TIMEOUT, TASK_FOREVER, &pollCoreStatus);

ActivityCoreClient::ActivityCoreClient(ClientId client, Scheduler& scheduler)
    : PeriphericalClient(client, scheduler) {
  scheduler.addTask(statusPollingTask);
}

void ActivityCoreClient::setup() {
  pollCoreStatus();
}

void ActivityCoreClient::init(BitmapId startupBitmap) {
  LOG("send display init request : startup.bitmap=%s.", startupBitmap);
  InitCoreRequest request(client, startupBitmap);
  sendRequest(CORE_MODULE_ID, &request, sizeof(InitCoreRequest));
}

void ActivityCoreClient::clearDisplay() {
  LOG("send clear display request.");
  ClearDisplayRequest request(client);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ClearDisplayRequest));
}

void ActivityCoreClient::putParam(ParameterId id, uint16_t value) {
  LOG("send display param request : id=%s, value=%d.", id, value);
  char text[10];
  itoa(value, text, 10);
  ParameterRequest request(client, id, text, 0);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ParameterRequest));
}

void ActivityCoreClient::putParam(ParameterId id, const char* value) {
  LOG("send display param request : id=%s, value=%s.", id, value);
  // TODO ---- chunks...
  ParameterRequest request(client, id, value, 0);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ParameterRequest));
}

void ActivityCoreClient::playSound(TrackId track) {
  LOG("send play sound request : track=%d.", track);
  PlaySoundRequest request(client, track);
  resumeStatus();
  sendRequest(CORE_MODULE_ID, &request, sizeof(PlaySoundRequest));
}

void ActivityCoreClient::stopSound() {
  LOG("send play stop request.");
  StopSoundRequest request(client);
  sendRequest(CORE_MODULE_ID, &request, sizeof(StopSoundRequest));
}

void ActivityCoreClient::displayMessage(MessageId id) {
  LOG("send display message request : id=%s.", id);
  DisplayMessageRequest request(client, id);
  sendRequest(CORE_MODULE_ID, &request, sizeof(DisplayMessageRequest));
}

void ActivityCoreClient::displayBitmap(BitmapId id) {
  LOG("send display bitmap request : id=%s.", id);
  DisplayBitmapRequest request(client, id);
  sendRequest(CORE_MODULE_ID, &request, sizeof(DisplayBitmapRequest));
}

void ActivityCoreClient::displayMenu(MenuId id) {
  LOG("send display menu request : id=%s.", id);
  DisplayMenuRequest request(client, id);
  sendRequest(CORE_MODULE_ID, &request, sizeof(DisplayMenuRequest));
}

void ActivityCoreClient::setStatusHandler(CoreStatusHandler* handler, unsigned long pollingDelay, unsigned long pollingTimeout) {
  if (handler != statusHandler) {
    statusHandler = handler;
    if (statusHandler) {
      statusPollingTask.enableDelayed(pollingDelay);
      statusPollingTask.setInterval(pollingTimeout);
    }
    else {
      statusPollingTask.disable();
    }
  }
}

void ActivityCoreClient::pauseStatus() {
  statusPollingTask.disable();
}

void ActivityCoreClient::resumeStatus(unsigned long pollingDelay) {
  if (!statusPollingTask.isEnabled() && statusHandler) {
    statusPollingTask.enableDelayed(pollingDelay);
  }
}

bool ActivityCoreClient::isStatusPaused() {
  return !statusPollingTask.isEnabled();
}

static void pollCoreStatus() {
  if (statusHandler) {
    CoreStatus status;
    PeriphericalClient::requestStatus(CORE_MODULE_ID, &status, sizeof(CoreStatus));
    if (lastPlayerState != status.playerState) {
      lastPlayerState = status.playerState;
      bool cancelHandler = statusHandler->playerStateChanged(lastPlayerState);
      if (cancelHandler) {
        statusPollingTask.disable();
      }
    }
  }
  else {
    statusPollingTask.disable();
  }
}
