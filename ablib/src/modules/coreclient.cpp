#include "coreclient.h"

static volatile bool statusAvailable = false;

static void IRAM_ATTR statusInterrupt() {
  statusAvailable = true;
}

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
  if (statusAvailable || millis() > heartbeat + CORECLIENT_HEARTBEAT_TIMEOUT) {
    heartbeat = millis();
    statusAvailable = false;
    processAvailableStatus();
  }
}

void ActivityCoreClient::init(ImageId startupImage) {
  LOG("send display init request : startup.image=%s.", startupImage);
  InitCoreRequest request(client, startupImage);
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

void ActivityCoreClient::playMusic(MusicId id) {
  LOG("send display message request : id=%s.", id);
  PlayMusicRequest request(client, id);
  sendRequest(CORE_MODULE_ID, &request, sizeof(PlayMusicRequest));
  blockCoreStatusProcessing(500);
}

void ActivityCoreClient::playSound(TrackId track) {
  playSound(client, track);
}

void ActivityCoreClient::playSound(TrackFolder folder, TrackId track) {
  LOG("send play sound request : folder=%d, track=%d.", folder, track);
  PlaySoundRequest request(client, folder, track);
  sendRequest(CORE_MODULE_ID, &request, sizeof(PlaySoundRequest));
  blockCoreStatusProcessing(500);
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

void ActivityCoreClient::displayImage(ImageId id) {
  LOG("send display image request : id=%s.", id);
  DisplayImageRequest request(client, id);
  sendRequest(CORE_MODULE_ID, &request, sizeof(DisplayImageRequest));
}

void ActivityCoreClient::displayMenu(MenuId id, uint8_t selectedIndex) {
  LOG("send display menu request : id=%s (%d).", id, selectedIndex);
  DisplayMenuRequest request(client, id, selectedIndex);
  sendRequest(CORE_MODULE_ID, &request, sizeof(DisplayMenuRequest));
}

DataStream ActivityCoreClient::readData(DataId id) {
  LOG("send read data request : id=%s.", id);
  ReadDataRequest request(client, id);
  sendRequest(CORE_MODULE_ID, &request, sizeof(ReadDataRequest));
  blockCoreStatusProcessing(2000);
  delay(200);
  return DataStream(client);
}

void ActivityCoreClient::setStatusHandler(CoreStatusHandler* handler) {
  statusHandler = handler;
}

void ActivityCoreClient::processAvailableStatus() {
  if (coreStatusBlocked) {
    if (millis() <= coreStatusBlocked) {
      LOG("status processing blocked");
      return;
    }
    coreStatusBlocked = 0;
  }

  CoreStatus status;
  requestCoreStatus(status);
  LOG("fetched core status: playerState=%d, webInput=0x%02X", status.playerState, status.webState.input);
  if (statusHandler) {
    statusHandler->handleCoreStatus(status);
  }
}

void ActivityCoreClient::blockCoreStatusProcessing(unsigned long timeout) {
  coreStatusBlocked = millis() + timeout;
}

void ActivityCoreClient::requestCoreStatus(CoreStatus& status) {
  requestStatus(CORE_MODULE_ID, StatusType::CORE_STATUS, &status, sizeof(CoreStatus));
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
