#include "displayservice.h"

#include "const.h"

DisplayService::DisplayService()
    : PeriphericalService(publishStatus),
      contexts(storage),
      display(storage) {
}

void DisplayService::setup(uint8_t module) {
  PeriphericalService::setup(module);
  delay(1500); /* wait a while to get power for storage */
  storage.setup();
  display.setup();
  rotary.setup();
  runner.setup();

  LOG("Display Service started.");
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
    case RequestType::DISPLAY_CLEAR:
      handleRequest((ClearDisplayRequest*)request);
      break;
    case RequestType::DISPLAY_PARAMETER:
      handleRequest((DisplayParameterRequest*)request);
      break;
    case RequestType::DISPLAY_MESSAGE:
      handleRequest((DisplayMessageRequest*)request);
      break;
    case RequestType::DISPLAY_MENU:
      handleRequest((DisplayMenuRequest*)request);
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
  RotaryEncoderWrapper::getStatus(status.rotaryStatus);
  PeriphericalService::sendStatus(&status, sizeof(DisplayStatus));
}

void DisplayService::handleRequest(InitDisplayRequest* request) {
  rotary.reset();
  BitmapDef def = contexts.get(request->client).getBitmapDef(request->startupBitmap);
  LOG("draw startup bitmap: id=%s, path=%s.", request->startupBitmap, def.path);
  auto& dsp = display;
  runner.schedule([&dsp, def]() { dsp.drawBitmap(def, 0, 0); });
}

void DisplayService::handleRequest(ClearDisplayRequest* request) {
  display.clear();
}

void DisplayService::handleRequest(DisplayParameterRequest* request) {
  LOG("put param: client=%d, id=%s, value=%s.", request->client, request->id, request->value);
  contexts.get(request->client).putParam(request->id, request->value, request->append);
}

void DisplayService::handleRequest(DisplayMessageRequest* request) {
  String message = contexts.get(request->client).formatMessage(request->messageId);
  LOG("draw message: id=%s, message=%s.", request->messageId, message.c_str());
  display.drawText(message.c_str());
}

void DisplayService::handleRequest(DisplayMenuRequest* request) {
  MenuDef def = contexts.get(request->client).getMenuDef(request->menuId);
  LOG("open menu: id=%s.", request->menuId);
  auto& dsp = display;
  runner.schedule([&dsp, def]() { dsp.drawMenu(def); });
}

void DisplayService::handleRequest(DisplayBitmapRequest* request) {
  BitmapDef def = contexts.get(request->client).getBitmapDef(request->bitmapId);
  LOG("draw bitmap: id=%s, path=%s.", request->bitmapId, def.path);
  auto& dsp = display;
  runner.schedule([&dsp, def]() { dsp.drawBitmap(def, 0, 0); });
}
