#include <Wire.h>

#include "ablib.h"

/* ========================================================================= */
/* PERIPHERICAL CLIENT                                                       */
/* ========================================================================= */

bool PeriphericalClient::sendRequest(uint8_t module, BaseRequest* request, uint8_t length) {
  LOG("send request: module=%d, type=%d, length=%d", module, (uint16_t)request->type, length);

  Wire.beginTransmission(module);
  Wire.write((uint8_t*)request, length);  // byte* -> uint8_t*
  Wire.endTransmission();

  LOG("sent peripherical request: %d bytes", length);
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

PeriphericalService* PeriphericalService::instance = nullptr;

PeriphericalService::PeriphericalService() {
  PeriphericalService::instance = this;
}

void PeriphericalService::setup(uint8_t module) {
  activityBoard.setup(module);
  Wire.onReceive(PeriphericalService::onReceive);
  Wire.onRequest(PeriphericalService::onRequest);
  LOG("peripherical service initialized.");
}

void PeriphericalService::loop() {
  if (currentRequest != nullptr) {
    uint8_t buffer[REQUEST_BUFFER_SIZE];  // byte -> uint8_t

#if defined(ARDUINO_ARCH_ESP32)
    noInterrupts();
#endif

    memcpy(buffer, currentRequest, REQUEST_BUFFER_SIZE);
    currentRequest = nullptr;  // Request als verarbeitet markieren

#if defined(ARDUINO_ARCH_ESP32)
    interrupts(); 
#endif

    processRequest((BaseRequest*)buffer);
  }
}

void PeriphericalService::readRequest(int length) {
  if (length > REQUEST_BUFFER_SIZE) {
    length = REQUEST_BUFFER_SIZE;  // Pufferüberlauf verhindern
  }

  uint8_t* p = (uint8_t*)&requestBuffer;
  memset(p, 0, REQUEST_BUFFER_SIZE);

  for (int i = 0; i < length; i++) {
    *p++ = Wire.read();
  }

  currentRequest = (BaseRequest*)requestBuffer;
}

bool PeriphericalService::sendStatus(BaseStatus* status, uint8_t length) {
  Wire.write((uint8_t*)status, length);
  return true;
}

void PeriphericalService::onReceive(int length) {
  if (PeriphericalService::instance) {
    PeriphericalService::instance->readRequest(length);
  }
}

void PeriphericalService::onRequest() {
  if (PeriphericalService::instance) {
    PeriphericalService::instance->publishStatus();
  }
}