#include "core.h"

#include <Arduino.h>
#include <Wire.h>

#define POLLING_TIMEOUT 500

/* somehow it takes a long time to start sound at DFPlayer on core module */
#define STATUS_HANDLER_BLOCK 1000

static void pollStatus();
static CoreStatusHandler* statusHandler = nullptr;
static Task statusPollingTask(POLLING_TIMEOUT, TASK_FOREVER, &pollStatus);

static void blockStatusHandler();
static unsigned long statusHandlerBlock = 0;

ActivityCoreClient::ActivityCoreClient(ClientId client, Scheduler& scheduler)
    : PeriphericalClient(client, scheduler) {
  scheduler.addTask(statusPollingTask);
}

void ActivityCoreClient::setup() {
  pollStatus();
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
  blockStatusHandler();
  resumeStatus();
  sendRequest(CORE_MODULE_ID, &request, sizeof(PlaySoundRequest));
}

void ActivityCoreClient::stopSound() {
  LOG("send play stop request.");
  blockStatusHandler();
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

void ActivityCoreClient::displayMenu(MenuId id, uint8_t selectedIndex) {
  LOG("send display menu request : id=%s (%d).", id, selectedIndex);
  DisplayMenuRequest request(client, id, selectedIndex);
  sendRequest(CORE_MODULE_ID, &request, sizeof(DisplayMenuRequest));
}

void ActivityCoreClient::setStatusHandler(CoreStatusHandler* handler, unsigned long pollingDelay, unsigned long pollingTimeout) {
  statusHandler = handler;
  statusPollingTask.disable();
  if (statusHandler) {
    LOG("set status handler: delay=%lu, poll=%lu", pollingDelay, pollingTimeout);
    statusPollingTask.setInterval(pollingTimeout);
    statusPollingTask.enableDelayed(pollingDelay);
  }
  else {
    LOG("status handler cleared");
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

static void pollStatus() {
  if (statusHandler) {
    CoreStatus status;
    PeriphericalClient::requestStatus(CORE_MODULE_ID, &status, sizeof(CoreStatus));
    if (!statusHandlerBlock || (millis() > statusHandlerBlock)) {
      // LOG("player=%d", status.playerState);
      bool cancelHandler = statusHandler->handleCoreStatus(status);
      if (cancelHandler) {
        // LOG("polling canceled")
        statusPollingTask.disable();
      }
    }
    else {
      // LOG("status handler blocked: player=%d", status.playerState);
    }
  }
  else {
    // LOG("no status handler")
    statusPollingTask.disable();
  }
}

static void blockStatusHandler() {
  // LOG("block status handler");
  statusHandlerBlock = millis() + STATUS_HANDLER_BLOCK;
}
