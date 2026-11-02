#include "core.h"

#include <Arduino.h>
#include <Wire.h>

/* ========================================================================= */
/*                                                     status polling config */
/* ========================================================================= */

#define POLLING_TIMEOUT 500

/* somehow it takes a long time to start sound at DFPlayer on core module */
#define STATUS_HANDLER_BLOCK 1000

static void pollStatus();
static ClientId pollingClient;
static CoreStatusHandler* statusHandler = nullptr;
static Task statusPollingTask(POLLING_TIMEOUT, TASK_FOREVER, &pollStatus);

static void blockStatusHandler();
static unsigned long statusHandlerBlock = 0;

/* ========================================================================= */
/*                                                core client implementation */
/* ========================================================================= */

ActivityCoreClient::ActivityCoreClient(ClientId client, Scheduler& scheduler)
    : PeriphericalClient(client, scheduler) {
  pollingClient = client;
  scheduler.addTask(statusPollingTask);
}

void ActivityCoreClient::setup() {
  pollStatus();
}

/* ========================================================================= */
/*                                                          general requests */
/* ========================================================================= */

void ActivityCoreClient::init(BitmapId startupBitmap) {
  LOG("send display init request : startup bitmap=%s.", startupBitmap);
  InitCoreRequest request(client, startupBitmap);
  sendRequest(CORE_MODULE_ID, &request, sizeof(InitCoreRequest));
}

/* ========================================================================= */
/*                                                        parameter requests */
/* ========================================================================= */

void ActivityCoreClient::putParameter(ParameterId id, uint16_t value) {
  LOG("send display param request : id=%s, value=%d.", id, value);
  char text[10];
  itoa(value, text, 10);
  ParameterRequest request(client, id, text, 0);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ParameterRequest));
}

void ActivityCoreClient::putParameter(ParameterId id, const char* value) {
  LOG("send display param request : id=%s, value=%s.", id, value);
  // TODO ---- chunks...
  ParameterRequest request(client, id, value, 0);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ParameterRequest));
}

/* ========================================================================= */
/*                                                            sound requests */
/* ========================================================================= */

void ActivityCoreClient::playSound(TrackId track) {
  LOG("send play sound request : track=%s", track);
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

/* ========================================================================= */
/*                                                          display requests */
/* ========================================================================= */

void ActivityCoreClient::clearDisplay() {
  LOG("send clear display request.");
  ClearDisplayRequest request(client);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ClearDisplayRequest));
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

/* ========================================================================= */
/*                                                    data transfer handling */
/* ========================================================================= */

DataStream ActivityCoreClient::readData(DataId id) {
  LOG("send read data request : id=%s.", id);
  ReadDataRequest request(client, id);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ReadDataRequest));
  delay(REQUEST_DATA_READ_DELAY);
  return DataStream(client);
}

DataStream::DataStream(ClientId client)
    : client(client) {
  bufferIndex = REQUEST_BUFFER_SIZE;
  memset(buffer, 0, REQUEST_BUFFER_SIZE);
}

int DataStream::read() {
  if (eof) {
    return -1;
  }

  if (bufferIndex >= REQUEST_BUFFER_SIZE) {
    memset(buffer, 0, REQUEST_BUFFER_SIZE);
    PeriphericalClient::requestStatus(CORE_MODULE_ID, client, StatusType::CORE_DATA, buffer, REQUEST_BUFFER_SIZE, REQUEST_DATA_RECEIVE_DELAY);
    DUMP(buffer, REQUEST_BUFFER_SIZE);
    bufferIndex = 0;
  }

  uint8_t c = buffer[bufferIndex];
  bufferIndex++;

  if (c == '\0') {
    eof = true;
    return -1;
  }

  return c;
}

int DataStream::available() {
  return eof ? 0 : 1;
}

int DataStream::peek() {
  LOG("peek");
  return -1;
}

size_t DataStream::write(uint8_t) {
  return 0;
}

void DataStream::flush() {
}

/* ========================================================================= */
/*                                             status polling implementation */
/* ========================================================================= */

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
    PeriphericalClient::requestStatus(CORE_MODULE_ID, pollingClient, StatusType::CORE_STATUS, &status, sizeof(CoreStatus));
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
