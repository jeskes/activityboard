#include "modules.h"

#include "requestqueue.h"

/* ========================================================================= */
/* PERIPHERICAL CLIENT                                                       */
/* ========================================================================= */

PeriphericalClient::PeriphericalClient(ClientId client, Scheduler& scheduler)
    : client(client) {
}

bool PeriphericalClient::sendRequest(ModuleId module, BaseRequest* request, uint8_t length) {
  // LOG("send request: module=%d, type=0x%04x, length=%d", module, (uint16_t)request->type, length);
  // DUMP(request, length);

  Wire.beginTransmission(module);
  Wire.write((uint8_t*)request, length);
  Wire.endTransmission();
  delay(REQUEST_SEND_DELAY);

  return true;
}

bool PeriphericalClient::requestStatus(uint8_t module, uint8_t client, StatusType type, uint8_t* buffer, uint8_t length, unsigned long receiveDelay) {
  // LOG("request status: module=%d, client=%d, type=%d", module, client, type);
  StatusRequest request(client, type);
  sendRequest(module, &request, sizeof(StatusRequest));
  delay(receiveDelay);
  int received = Wire.requestFrom(module, length);
  if (received == length) {
    for (int idx = 0; idx < length; idx++) {
      buffer[idx] = Wire.read();
    }
    // DUMP(buffer, length);
    return true;
  }
  else {
    while (Wire.available()) {
      Wire.read();
    }
    return false;
  }
}

bool PeriphericalClient::requestStatus(uint8_t module, ClientId client, StatusType type, BaseStatus* status, uint8_t length) {
  return requestStatus(module, client, type, (uint8_t*)status, length);
}

/* ========================================================================= */
/* PERIPHERICAL SERVICE                                                      */
/* ========================================================================= */

static void onRequest();
static void onReceive(int length);

static void (*statusSender)();

static RequestQueue requestQueue;

PeriphericalService::PeriphericalService(void (*sender)()) {
  statusSender = sender;
}

void PeriphericalService::setup(uint8_t module) {
  activityBoard.setup(module);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  LOG("peripherical service initialized: id=%d.", module);
}

void PeriphericalService::loop() {
  uint8_t buffer[REQUEST_BUFFER_SIZE];
  if (requestQueue.pull(buffer)) {
    processRequest((BaseRequest*)buffer);
  }
}

bool PeriphericalService::sendStatus(BaseStatus* status, uint8_t length) {
  // DUMP(status, length);
  Wire.write((uint8_t*)status, length);
  return true;
}

static void onReceive(int length) {
  requestQueue.push(length);
}

static void onRequest() {
  if (statusSender) {
    statusSender();
  }
}

/* ========================================================================= */
/*                                                                GadgetBase */
/* ========================================================================= */

void GadgetBase::setup() {
  delay(GADGET_STARTUP_DELAY);
  activityBoard.setup();
  LOG("gadget setup done.");
}

void GadgetBase::loop() {
  scheduler.execute();
}
