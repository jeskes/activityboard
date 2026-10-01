#include <Arduino.h>
#include <Wire.h>

#include "ablib.h"

ActivityDisplayClient::ActivityDisplayClient() {
}

void ActivityDisplayClient::setup(uint16_t folderId) {
  LOG("setup display module : folder=%d.", folderId);
  SetupDisplayRequest request(folderId);
  sendRequest(DISPLAY_MODULE_ID, &request, sizeof(SetupDisplayRequest));
}

void ActivityDisplayClient::clearDisplay() {
  LOG("send clear display request.");
  ClearDisplayRequest request;
  sendRequest(DISPLAY_MODULE_ID, &request, sizeof(ClearDisplayRequest));
}

void ActivityDisplayClient::displayMessage(uint16_t messageId) {
  LOG("send display message request : id=%d.", messageId);
  DisplayMessageRequest request(messageId);
  sendRequest(DISPLAY_MODULE_ID, &request, sizeof(DisplayMessageRequest));
}

void ActivityDisplayClient::displayBitmap(uint16_t bitmapId) {
  LOG("send display bitmap request : id=%d.", bitmapId);
  DisplayBitmapRequest request(bitmapId);
  sendRequest(DISPLAY_MODULE_ID, &request, sizeof(DisplayBitmapRequest));
}
