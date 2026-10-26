#include "coreservice.h"

#include "const.h"

CoreService::CoreService()
    : PeriphericalService(publishStatus),
      player(ledRing),
      contexts(storage),
      display(storage, ledRing) {
}

void CoreService::setup(uint8_t module) {
  PeriphericalService::setup(module);
  // delay(1500); /* wait a while to get power for storage */

  pinMode(PLAYER_BUSY, INPUT_PULLUP);

  ledRing.setup();
  storage.setup();
  display.setup();
  runner.setup();
  player.setup();

  LOG("Display Service started.");
}

void CoreService::loop() {
  PeriphericalService::loop();
  player.loop();
  display.loop();
}

void CoreService::processRequest(BaseRequest* request) {
  LOG("request arrived: %d", request->type);
  switch (request->type) {
    case RequestType::INIT:
      handleRequest((InitCoreRequest*)request);
      break;
    case RequestType::PARAMETER:
      handleRequest((ParameterRequest*)request);
      break;
    case RequestType::PLAY_SOUND:
      handleRequest((PlaySoundRequest*)request);
      break;
    case RequestType::STOP_SOUND:
      handleRequest((StopSoundRequest*)request);
      break;
    case RequestType::CLEAR_DISPLAY:
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

void CoreService::publishStatus() {
  CoreStatus status;
  status.playerState = Player::playerState;
  PeriphericalService::sendStatus(&status, sizeof(CoreStatus));
}

void CoreService::handleRequest(InitCoreRequest* request) {
  BitmapDef def = contexts.get(request->client).getBitmapDef(request->startupBitmap);
  LOG("draw startup bitmap: id=%s, path=%s.", request->startupBitmap, def.path);
  auto& dsp = display;
  runner.schedule([&dsp, def]() { dsp.drawBitmap(def, 0, 0); });
}

void CoreService::handleRequest(ClearDisplayRequest* request) {
  display.clear();
}

void CoreService::handleRequest(ParameterRequest* request) {
  LOG("put param: client=%d, id=%s, value=%s.", request->client, request->id, request->value);
  contexts.get(request->client).putParam(request->id, request->value, request->append);
}

void CoreService::handleRequest(DisplayMessageRequest* request) {
  String message = contexts.get(request->client).formatMessage(request->messageId);
  LOG("draw message: id=%s, message=%s.", request->messageId, message.c_str());
  display.drawText(message.c_str());
}

void CoreService::handleRequest(DisplayMenuRequest* request) {
  MenuDef def = contexts.get(request->client).getMenuDef(request->menuId);
  LOG("open menu: id=%s.", request->menuId);
  auto& dsp = display;
  runner.schedule([&dsp, def]() { dsp.drawMenu(def); });
}

void CoreService::handleRequest(DisplayBitmapRequest* request) {
  BitmapDef def = contexts.get(request->client).getBitmapDef(request->bitmapId);
  LOG("draw bitmap: id=%s, path=%s.", request->bitmapId, def.path);
  auto& dsp = display;
  runner.schedule([&dsp, def]() { dsp.drawBitmap(def, 0, 0); });
}

void CoreService::handleRequest(PlaySoundRequest* request) {
  player.play(request->client, request->track);
}

void CoreService::handleRequest(StopSoundRequest* request) {
  player.stop();
}
