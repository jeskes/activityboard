#include <Arduino.h>
#include <Wire.h>

#include "ablib.h"

ActivityDisplayClient::ActivityDisplayClient() {
}

void ActivityDisplayClient::init(uint16_t clientId) {
  LOG("init display module : client=%d.", clientId);
  InitDisplayRequest request(clientId);
  sendRequest(DISPLAY_MODULE_ID, &request, sizeof(InitDisplayRequest));
}

void ActivityDisplayClient::clearDisplay() {
  LOG("send clear display request.");
  ClearDisplayRequest request;
  sendRequest(DISPLAY_MODULE_ID, &request, sizeof(ClearDisplayRequest));
}

void ActivityDisplayClient::putParam(const char* name, uint16_t value) {
  LOG("send display param request : id=%s, value=%d.", name, value);
  char text[10];
  itoa(value, text,10);
  DisplayParameterRequest request(name, text, 0);
  sendRequest(DISPLAY_MODULE_ID, &request, sizeof(DisplayParameterRequest));
}

void ActivityDisplayClient::putParam(const char* name, const char* value) {
  LOG("send display param request : id=%s, value=%s.", name, value);
  // TODO ---- chunks...
  DisplayParameterRequest request(name, value, 0);
  sendRequest(DISPLAY_MODULE_ID, &request, sizeof(DisplayParameterRequest));
}

void ActivityDisplayClient::displayMessage(const char* messageId) {
  LOG("send display message request : id=%s.", messageId);
  DisplayMessageRequest request(messageId);
  sendRequest(DISPLAY_MODULE_ID, &request, sizeof(DisplayMessageRequest));
}

void ActivityDisplayClient::displayBitmap(const char* bitmapId) {
  LOG("send display bitmap request : id=%s.", bitmapId);
  DisplayBitmapRequest request(bitmapId);
  sendRequest(DISPLAY_MODULE_ID, &request, sizeof(DisplayBitmapRequest));
}
