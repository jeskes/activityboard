#include "coreclient.h"

#define STATUS_POLLING_TIMEOUT 500
#define DEFAULT_STATUS_POLLING_DELAY 50
#define PLAYER_STATUS_POLLING_DELAY 1200

static ClientId client;
static void pollStatus();
static CoreStatusHandler* statusHandler = nullptr;
static Task statusPollingTask(STATUS_POLLING_TIMEOUT, TASK_FOREVER, &pollStatus);

ActivityCoreClient::ActivityCoreClient(ClientId client, Scheduler& scheduler)
    : PeriphericalClient(client, scheduler) {
  ::client = client;
  statusPollingTask.disable();
  scheduler.addTask(statusPollingTask);
}

void ActivityCoreClient::setup() {
}

void ActivityCoreClient::init(BitmapId startupBitmap) {
  LOG("send display init request : startup.bitmap=%s.", startupBitmap);
  InitCoreRequest request(client, startupBitmap);
  sendRequest(CORE_MODULE_ID, &request, sizeof(InitCoreRequest), DEFAULT_STATUS_POLLING_DELAY);
}

void ActivityCoreClient::clearDisplay() {
  LOG("send clear display request.");
  ClearDisplayRequest request(client);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ClearDisplayRequest), DEFAULT_STATUS_POLLING_DELAY);
}

void ActivityCoreClient::putParam(ParameterId id, uint16_t value) {
  LOG("send display param request : id=%s, value=%d.", id, value);
  char text[10];
  itoa(value, text, 10);
  ParameterRequest request(client, id, text, 0);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ParameterRequest), DEFAULT_STATUS_POLLING_DELAY);
}

void ActivityCoreClient::putParam(ParameterId id, const char* value) {
  LOG("send display param request : id=%s, value=%s.", id, value);
  // TODO ---- chunks...
  ParameterRequest request(client, id, value, 0);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ParameterRequest), DEFAULT_STATUS_POLLING_DELAY);
}

void ActivityCoreClient::playSound(TrackId track) {
  LOG("send play sound request : track=%s.", track);
  PlaySoundRequest request(client, track);
  sendRequest(CORE_MODULE_ID, &request, sizeof(PlaySoundRequest), PLAYER_STATUS_POLLING_DELAY);
}

void ActivityCoreClient::stopSound() {
  LOG("send play stop request.");
  StopSoundRequest request(client);
  sendRequest(CORE_MODULE_ID, &request, sizeof(StopSoundRequest), PLAYER_STATUS_POLLING_DELAY);
}

void ActivityCoreClient::displayMessage(MessageId id) {
  LOG("send display message request : id=%s.", id);
  DisplayMessageRequest request(client, id);
  sendRequest(CORE_MODULE_ID, &request, sizeof(DisplayMessageRequest), DEFAULT_STATUS_POLLING_DELAY);
}

void ActivityCoreClient::displayBitmap(BitmapId id) {
  LOG("send display bitmap request : id=%s.", id);
  DisplayBitmapRequest request(client, id);
  sendRequest(CORE_MODULE_ID, &request, sizeof(DisplayBitmapRequest), DEFAULT_STATUS_POLLING_DELAY);
}

void ActivityCoreClient::displayMenu(MenuId id, uint8_t selectedIndex) {
  LOG("send display menu request : id=%s (%d).", id, selectedIndex);
  DisplayMenuRequest request(client, id, selectedIndex);
  sendRequest(CORE_MODULE_ID, &request, sizeof(DisplayMenuRequest), DEFAULT_STATUS_POLLING_DELAY);
}

DataStream ActivityCoreClient::readData(DataId id) {
  LOG("send read data request : id=%s.", id);
  ReadDataRequest request(client, id);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ReadDataRequest), false);
  delay(200);
  return DataStream();
}

void ActivityCoreClient::snoozeStatus(unsigned long delay) {
  statusPollingTask.disable();
  statusPollingTask.enableDelayed(delay);
}

void ActivityCoreClient::setStatusHandler(CoreStatusHandler* handler) {
  statusHandler = handler;
}

bool ActivityCoreClient::sendRequest(ModuleId module, BaseRequest* request, uint8_t length, unsigned long snoozeStatusDelay) {
  snoozeStatus(snoozeStatusDelay);
  return PeriphericalClient::sendRequest(module, request, length);
}

DataStream::DataStream() {
  bufferIndex = REQUEST_BUFFER_SIZE;
}

int DataStream::read() {
  if (bufferIndex >= REQUEST_BUFFER_SIZE) {
    memset(buffer, 0, REQUEST_BUFFER_SIZE);
    PeriphericalClient::requestStatus(CORE_MODULE_ID, client, StatusType::CORE_DATA, (BaseStatus*)buffer, REQUEST_BUFFER_SIZE);
    bufferIndex = 0;
  }

  return !buffer[bufferIndex] ? 1 : buffer[bufferIndex++];
}

void DataStream::close() {
  while (read() != -1)
    ;
}

/* ========================================================================= */
/*                                                       core status polling */
/* ========================================================================= */

static void pollStatus() {
  if (statusHandler) {
    CoreStatus status;
    PeriphericalClient::requestStatus(CORE_MODULE_ID, ::client, StatusType::CORE_STATUS, &status, sizeof(CoreStatus));
    bool cancelHandler = statusHandler->handleCoreStatus(status);
    if (cancelHandler) {
      statusPollingTask.disable();
    }
  }
  else {
    statusPollingTask.disable();
  }
}
