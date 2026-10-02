#include "displayservice.h"

#include "ablib.h"

#define TFT_SDI 23
#define TFT_SCK 18

#define SD_CS 15

DisplayService::DisplayService()
    : context(storage),
      display(storage) {
}

void DisplayService::setup(uint8_t module) {
  PeriphericalService::setup(module);
  storage.setup();
  context.setup();
  display.setup();
  LOG("Display Module started.");
}

void DisplayService::loop() {
  PeriphericalService::loop();
}

void DisplayService::processRequest(BaseRequest* request) {
  switch (request->type) {
    case RequestType::DISPLAY_INIT:
      handleRequest((InitDisplayRequest*)request);
      break;
    case RequestType::DISPLAY_PARAMETER:
      handleRequest((DisplayParameterRequest*)request);
      break;
    case RequestType::DISPLAY_CLEAR:
      handleRequest((ClearDisplayRequest*)request);
      break;
    case RequestType::DISPLAY_MESSAGE:
      handleRequest((DisplayMessageRequest*)request);
      break;
    case RequestType::DISPLAY_BITMAP:
      handleRequest((DisplayBitmapRequest*)request);
      break;
    default:
      break;
  }
}

void DisplayService::publishStatus() {
  DisplayStatus status;
  PeriphericalService::sendStatus(&status, sizeof(DisplayStatus));
}

void DisplayService::handleRequest(InitDisplayRequest* request) {
  LOG("init display: client=%d", request->clientId);
  context.init(request->clientId);
}

void DisplayService::handleRequest(ClearDisplayRequest* request) {
	display.clear();
}

void DisplayService::handleRequest(DisplayParameterRequest* request) {
  context.putParam(request->name, request->value, request->append);
}

void DisplayService::handleRequest(DisplayMessageRequest* request) {
  String message = context.formatMessage(request->messageId);
  display.drawText(message.c_str());
}

void DisplayService::handleRequest(DisplayBitmapRequest* request) {
  const char* path = context.getBitmapPath(request->bitmapId).c_str();
  display.drawBitmap(path);
}