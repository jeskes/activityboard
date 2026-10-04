#include <Arduino.h>
#include <Wire.h>

#include "ablib.h"

ActivityDisplayClient::ActivityDisplayClient(ClientId client)
    : PeriphericalClient(client) {
}

void ActivityDisplayClient::clearDisplay() {
  LOG("send clear display request.");
  ClearDisplayRequest request(client);
  sendRequest(DISPLAY_MODULE_ID, &request, sizeof(ClearDisplayRequest));
}

void ActivityDisplayClient::putParam(ParameterId id, uint16_t value) {
  LOG("send display param request : id=%s, value=%d.", id, value);
  char text[10];
  itoa(value, text, 10);
  DisplayParameterRequest request(client, id, text, 0);
  sendRequest(DISPLAY_MODULE_ID, &request, sizeof(DisplayParameterRequest));
}

void ActivityDisplayClient::putParam(ParameterId id, const char* value) {
  LOG("send display param request : id=%s, value=%s.", id, value);
  // TODO ---- chunks...
  DisplayParameterRequest request(client, id, value, 0);
  sendRequest(DISPLAY_MODULE_ID, &request, sizeof(DisplayParameterRequest));
}

void ActivityDisplayClient::displayMessage(MessageId id) {
  LOG("send display message request : id=%s.", id);
  DisplayMessageRequest request(client, id);
  sendRequest(DISPLAY_MODULE_ID, &request, sizeof(DisplayMessageRequest));
}

void ActivityDisplayClient::displayBitmap(BitmapId id) {
  LOG("send display bitmap request : id=%s.", id);
  DisplayBitmapRequest request(client, id);
  sendRequest(DISPLAY_MODULE_ID, &request, sizeof(DisplayBitmapRequest));
}
