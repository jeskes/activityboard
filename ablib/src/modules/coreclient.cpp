#include "coreclient.h"

bool ActivityCoreClient::statusAvailable = false;

ActivityCoreClient::ActivityCoreClient(ClientId client)
    : PeriphericalClient(client),
      statusHandler(nullptr) {
  ActivityCoreClient::client = client;
}

void ActivityCoreClient::setup(uint8_t irq) {
  pinMode(irq, INPUT);
  attachInterrupt(digitalPinToInterrupt(irq), statusInterrupt, FALLING);
  LOG("attached irq: pin=%d", irq);
}

void ActivityCoreClient::loop() {
  if (statusAvailable) {
    statusAvailable = false;
    processAvailableStatus();
  }
}

void ActivityCoreClient::init(BitmapId startupBitmap) {
  // LOG("send display init request : startup.bitmap=%s.", startupBitmap);
  InitCoreRequest request(client, startupBitmap);
  sendRequest(CORE_MODULE_ID, &request, sizeof(InitCoreRequest));
}

void ActivityCoreClient::clearDisplay() {
  // LOG("send clear display request.");
  ClearDisplayRequest request(client);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ClearDisplayRequest));
}

void ActivityCoreClient::putParam(ParameterId id, uint16_t value) {
  // LOG("send display param request : id=%s, value=%d.", id, value);
  char text[10];
  itoa(value, text, 10);
  ParameterRequest request(client, id, text, 0);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ParameterRequest));
}

void ActivityCoreClient::putParam(ParameterId id, const char* value) {
  // LOG("send display param request : id=%s, value=%s.", id, value);
  // TODO ---- chunks...
  ParameterRequest request(client, id, value, 0);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ParameterRequest));
}

void ActivityCoreClient::playSound(TrackId track) {
  playSound(client, track);
}

void ActivityCoreClient::playSound(TrackFolder folder, TrackId track) {
  LOG("send play sound request : folder=%d, track=%d.", folder, track);
  PlaySoundRequest request(client, folder, track);
  sendRequest(CORE_MODULE_ID, &request, sizeof(PlaySoundRequest));
}

void ActivityCoreClient::stopSound() {
  LOG("send play stop request.");
  StopSoundRequest request(client);
  sendRequest(CORE_MODULE_ID, &request, sizeof(StopSoundRequest));
}

void ActivityCoreClient::displayMessage(MessageId id) {
  // LOG("send display message request : id=%s.", id);
  DisplayMessageRequest request(client, id);
  sendRequest(CORE_MODULE_ID, &request, sizeof(DisplayMessageRequest));
}

void ActivityCoreClient::displayBitmap(BitmapId id) {
  // LOG("send display bitmap request : id=%s.", id);
  DisplayBitmapRequest request(client, id);
  sendRequest(CORE_MODULE_ID, &request, sizeof(DisplayBitmapRequest));
}

void ActivityCoreClient::displayMenu(MenuId id, uint8_t selectedIndex) {
  // LOG("send display menu request : id=%s (%d).", id, selectedIndex);
  DisplayMenuRequest request(client, id, selectedIndex);
  sendRequest(CORE_MODULE_ID, &request, sizeof(DisplayMenuRequest));
}

DataStream ActivityCoreClient::readData(DataId id) {
  // LOG("send read data request : id=%s.", id);
  ReadDataRequest request(client, id);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ReadDataRequest));
  delay(200);
  return DataStream(client);
}

void ActivityCoreClient::setStatusHandler(CoreStatusHandler* handler) {
  statusHandler = handler;
}

void ActivityCoreClient::processAvailableStatus() {
  CoreStatus status;
  requestCoreStatus(status);
  if (statusHandler) {
    statusHandler->handleCoreStatus(status);
  }
}

void ActivityCoreClient::requestCoreStatus(CoreStatus& status) {
  requestStatus(CORE_MODULE_ID, StatusType::CORE_STATUS, &status, sizeof(CoreStatus));
}

void IRAM_ATTR ActivityCoreClient::statusInterrupt() {
  LOG("interrupted");
  statusAvailable = true;
}

DataStream::DataStream(ClientId client)
    : client(client) {
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
