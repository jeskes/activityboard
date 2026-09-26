#include <Wire.h>

#include "ablib.h"

/* peripherical client */

bool PeriphericalClient::sendRequest(int module, BaseRequest* request, int length) {
  LOG("send request: module=%d, type=%d, length=%d", module, request->type, length);
  Wire.beginTransmission(module);
  Wire.write((byte*)request, length);
  Wire.endTransmission();
  LOG("sent peripherical request: %d bytes", length);
  return true;
}

bool PeriphericalClient::requestStatus(int module, BaseStatus* status, int length) {
  int received = Wire.requestFrom(module, length);
  if (received == length) {
    uint8_t* buffer = (uint8_t*)status;
    for (size_t idx = 0; idx < length; idx++) {
      buffer[idx] = Wire.read();
    }
    return true;
  }
  else {
    // skip invalid response
    while (Wire.available())
      Wire.read();
    return false;
  }
}

/* peripherical service */

PeriphericalService::PeriphericalService() {
  PeriphericalService::instance = this;
}

void PeriphericalService::setup(int module) {
  activityBoard.init(module);
  Wire.onReceive(PeriphericalService::onReceive);
  Wire.onRequest(PeriphericalService::onRequest);
  LOG("peripherical service initialized.");
}

void PeriphericalService::loop() {
  if (currentRequest) {
    processRequest(currentRequest);
    currentRequest = nullptr;
  }
}

void PeriphericalService::readRequest(int length) {
  byte* p = (byte*)&requestBuffer;
  memset(p, 0, REQUEST_BUFFER_SIZE);

  for (unsigned int i = 0; i < length; i++) {
    *p++ = Wire.read();
  }
  currentRequest = (BaseRequest*)&requestBuffer;
}
bool PeriphericalService::sendStatus(BaseStatus* status, int length) {
  Wire.write((uint8_t*)status, length);
  return true;
}

void PeriphericalService::onReceive(int length) {
  PeriphericalService::instance->readRequest(length);
}

void PeriphericalService::onRequest() {
  PeriphericalService::instance->publishStatus();
}
