#include <Wire.h>

#include "ablib.h"

/* ========================================================================= */
/* PERIPHERICAL CLIENT                                                       */
/* ========================================================================= */

bool PeriphericalClient::sendRequest(uint8_t module, BaseRequest* request, uint8_t length) {
  LOG("send request: module=%d, type=0x%04x, length=%d", module, (uint16_t)request->type, length);

  Wire.beginTransmission(module);
  Wire.write((uint8_t*)request, length);
  Wire.endTransmission();

  LOG("sent peripherical request: %d bytes", length);
  // TODO --- throttle or request buffer at service side
  delay(100);

  return true;
}

bool PeriphericalClient::requestStatus(uint8_t module, BaseStatus* status, uint8_t length) {
  int received = Wire.requestFrom((uint8_t)module, (uint8_t)length);
  if (received == length) {
    uint8_t* buffer = (uint8_t*)status;
    for (int idx = 0; idx < length; idx++) {
      buffer[idx] = Wire.read();
    }
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
static BaseRequest* currentRequest = nullptr;
static uint8_t requestBuffer[REQUEST_BUFFER_SIZE];

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
  if (currentRequest != nullptr) {
    uint8_t buffer[REQUEST_BUFFER_SIZE];
#if defined(ARDUINO_ARCH_ESP32)
    // noInterrupts();
#endif
    memcpy(buffer, currentRequest, REQUEST_BUFFER_SIZE);
    currentRequest = nullptr;
#if defined(ARDUINO_ARCH_ESP32)
    // interrupts();
#endif
    processRequest((BaseRequest*)buffer);
  }
}

bool PeriphericalService::sendStatus(BaseStatus* status, uint8_t length) {
  /* size_t size = */ Wire.write((uint8_t*)status, length);
  // LOG("send status : length=%d, size=%d", length, size);
  return true;
}

static void onReceive(int length) {
  if (currentRequest) {
    LOG("no receive");
    return;
  }

  if (length > REQUEST_BUFFER_SIZE) {
    length = REQUEST_BUFFER_SIZE;
  }

  uint8_t* p = (uint8_t*)&requestBuffer;
  memset(p, 0, REQUEST_BUFFER_SIZE);

  for (int i = 0; i < length; i++) {
    *p++ = (uint8_t)Wire.read();
  }

  currentRequest = (BaseRequest*)requestBuffer;
}

static void onRequest() {
  if (statusSender) {
    statusSender();
  }
}
