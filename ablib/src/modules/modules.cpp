#include "modules.h"

#include "requestqueue.h"

/* ========================================================================= */
/* PERIPHERICAL CLIENT                                                       */
/* ========================================================================= */

PeriphericalClient::PeriphericalClient(ClientId client, Scheduler& scheduler)
    : client(client) {
  /* scheduler not used yet */
}

bool PeriphericalClient::sendRequest(ModuleId module, BaseRequest* request, uint8_t length) {
  // LOG("send request: module=%d, type=0x%04x, length=%d", module, (uint16_t)request->type, length);
  // DUMP(request, length);

  /* maybe put send in a task/queue using scheduler... */

  uint8_t buffer[REQUEST_BUFFER_SIZE] = {0};
  memcpy(buffer, request, length);
  DUMP(buffer, REQUEST_BUFFER_SIZE);

  Wire.beginTransmission(module);
  Wire.write(buffer, REQUEST_BUFFER_SIZE);
  Wire.endTransmission();

  // LOG("sent peripherical request: %d bytes", length);
  // TODO --- throttle or request buffer at service side
  delay(50);

  return true;
}

bool PeriphericalClient::requestStatus(uint8_t module, BaseStatus* status, uint8_t length) {
  int received = Wire.requestFrom(module, length);
  if (received == length) {
    uint8_t* buffer = (uint8_t*)status;
    for (int idx = 0; idx < length; idx++) {
      buffer[idx] = Wire.read();
    }
    // DUMP(status, length);
    return true;
  }
  else {
    while (Wire.available()) {
      Wire.read();
    }
    return false;
  }
}

/* ========================================================================= */
/* PERIPHERICAL SERVICE                                                      */
/* ========================================================================= */

static void onRequest();
static void onReceive(int length);

static void (*statusSender)();

RequestQueue requestQueue;

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
  static uint8_t buffer[REQUEST_BUFFER_SIZE];
  if (requestQueue.pull(buffer)) {
    processRequest((BaseRequest*)buffer);
  }
}

bool PeriphericalService::sendStatus(BaseStatus* status, uint8_t length) {
  size_t size = Wire.write((uint8_t*)status, length);
  // LOG("send status : length=%d, size=%d", length, size);
  // DUMP(status, length);
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
  delay(1000);
  activityBoard.setup();
  LOG("gadget setup done.");
}

void GadgetBase::loop() {
  scheduler.execute();
}
