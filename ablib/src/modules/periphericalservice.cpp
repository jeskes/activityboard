#include "periphericalservice.h"

static void onReceive(int length);
static RequestQueue requestQueue;

PeriphericalService::PeriphericalService() {
  requestQueue.setHandler(this);
}

void PeriphericalService::setup(uint8_t module) {
  activityBoard.setup(module);
  Wire.onReceive(onReceive);
  LOG("peripherical service initialized: id=%d.", module);
}

void PeriphericalService::loop() {
  static uint8_t buffer[REQUEST_BUFFER_SIZE];
  if (requestQueue.pull(buffer)) {
    processRequest((BaseRequest*)buffer);
  }
}

bool PeriphericalService::sendStatus(BaseStatus* status, uint8_t length) {
#ifdef ARDUINO_ARCH_ESP32
  size_t written = Wire.slaveWrite((uint8_t*)status, length);
#else
  size_t written = Wire.write((uint8_t*)status, length);
#endif
  DUMP(status, length);
  return written == length;
}

static void onReceive(int length) {
  requestQueue.push(length);
}
