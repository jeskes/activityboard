#include "displayservice.h"

#include "const.h"

DisplayService::DisplayService()
    : PeriphericalService(publishStatus),
      contexts(storage),
      display(storage) {
}

void DisplayService::setup(uint8_t module) {
  PeriphericalService::setup(module);

  pinMode(TFT_BL, OUTPUT);
  activityControls.blink(TFT_BL, 10, 300);
  digitalWrite(TFT_BL, HIGH);

  storage.setup();
  display.setup();
  runner.setup();

  LOG("Display Module started.");
}

void DisplayService::loop() {
  PeriphericalService::loop();
}

void DisplayService::processRequest(BaseRequest* request) {
  LOG("request arrived: %d", request->type);
  switch (request->type) {
    case RequestType::DISPLAY_PARAMETER:
      handleRequest((DisplayParameterRequest*)request);
      break;
    case RequestType::DISPLAY_CLEAR:
      handleRequest((ClearDisplayRequest*)request);
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
  PeriphericalService::sendStatus(&status, sizeof(DisplayStatus));
}

void DisplayService::handleRequest(ClearDisplayRequest* request) {
  display.clear();
}

void DisplayService::handleRequest(DisplayParameterRequest* request) {
  LOG("DisplayService: put param: client=%d, id=%s, value=%s.", request->client, request->id, request->value);
  contexts.get(request->client).putParam(request->id, request->value, request->append);
}

void DisplayService::handleRequest(DisplayMessageRequest* request) {
  String message = contexts.get(request->client).formatMessage(request->messageId);
  LOG("DisplayService: draw message: id=%s, message=%s.", request->messageId, message.c_str());
  display.drawText(message.c_str());
}

void DisplayService::handleRequest(DisplayMenuRequest* request) {
  MenuDef def = contexts.get(request->client).getMenuDef(request->menuId);
  LOG("DisplayService: open menu: id=%s.", request->menuId);
  auto& dsp = display;
  runner.schedule([&dsp, def]() { dsp.drawMenu(def); });
}

void DisplayService::handleRequest(DisplayBitmapRequest* request) {
  BitmapDef def = contexts.get(request->client).getBitmapDef(request->bitmapId);
  LOG("DisplayService: draw bitmap: id=%s, path=%s.", request->bitmapId, def.path);
  auto& dsp = display;
  runner.schedule([&dsp, def]() { dsp.drawBitmap(def, 0, 0); });
}
