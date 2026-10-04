#include "displayservice.h"

#include "const.h"

DisplayService::DisplayService()
    : PeriphericalService(publishStatus),
      context(storage),
      display(storage) {
}

void DisplayService::setup(uint8_t module) {
  PeriphericalService::setup(module);

  pinMode(TFT_BL, OUTPUT);
  activityControls.blink(TFT_BL, 10, 300);
  digitalWrite(TFT_BL, HIGH);

  storage.setup();
  context.setup();
  display.setup();

  LOG("Display Module started.");
}

void DisplayService::loop() {
  PeriphericalService::loop();
}

void DisplayService::processRequest(BaseRequest* request) {
  LOG("request arrived: %d", request->type);
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
  LOG("init display: client=%d.", request->clientId);
  context.init(request->clientId);
}

void DisplayService::handleRequest(ClearDisplayRequest* request) {
  display.clear();
}

void DisplayService::handleRequest(DisplayParameterRequest* request) {
  LOG("DisplayService: put param: name=%s, value=%s.", request->name, request->value);
  context.putParam(request->name, request->value, request->append);
}

void DisplayService::handleRequest(DisplayMessageRequest* request) {
  String message = context.formatMessage(request->messageId);
  LOG("DisplayService: draw message: id=%s, message=%s.", request->messageId, message.c_str());
  display.drawText(message.c_str());
}

void DisplayService::handleRequest(DisplayBitmapRequest* request) {
  const char* path = context.getBitmapPath(request->bitmapId).c_str();
  LOG("DisplayService: draw bitmap: id=%s, path=%s.", request->bitmapId, path ? path : "not found");
  display.drawBitmap(path, 0, 0);
}