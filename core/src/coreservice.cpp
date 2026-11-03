#include "coreservice.h"

#include "const.h"

StatusType CoreService::requestedStatus = StatusType::UNKNOWN;

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

  requestQueue.setHandler(handleRequest);

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

/* ========================================================================= */
/*                                            request handling (onReceive()) */
/* ========================================================================= */

void CoreService::processRequest(BaseRequest* request) {
  // LOG("request arrived: client=%d, type=0x%X", request->client, request->type);
  switch (request->type) {
    case RequestType::STATUS:
      processRequest((StatusRequest*)request);
      break;
    case RequestType::INIT:
      processRequest((InitCoreRequest*)request);
      break;
    case RequestType::PARAMETER:
      processRequest((ParameterRequest*)request);
      break;
    case RequestType::PLAY_SOUND:
      processRequest((PlaySoundRequest*)request);
      break;
    case RequestType::STOP_SOUND:
      processRequest((StopSoundRequest*)request);
      break;
    case RequestType::CLEAR_DISPLAY:
      processRequest((ClearDisplayRequest*)request);
      break;
    case RequestType::DISPLAY_MESSAGE:
      processRequest((DisplayMessageRequest*)request);
      break;
    case RequestType::DISPLAY_MENU:
      processRequest((DisplayMenuRequest*)request);
      break;
    case RequestType::DISPLAY_BITMAP:
      processRequest((DisplayBitmapRequest*)request);
      break;
    case RequestType::READ_DATA:
      processRequest((ReadDataRequest*)request);
      break;
    default:
      break;
  }
}

void CoreService::processRequest(StatusRequest* request) {
  // LOG("requested status: %d", request->type);
  requestedStatus = request->statusType;
}

void CoreService::processRequest(InitCoreRequest* request) {
  BitmapDef def = contexts.get(request->client).getBitmapDef(request->startupBitmap);
  LOG("draw startup bitmap: id=%s, path=%s.", request->startupBitmap, def.path);
  auto& dsp = display;
  runner.schedule([&dsp, def]() { dsp.drawBitmap(def, 0, 0); });
}

void CoreService::processRequest(ClearDisplayRequest* request) {
  display.clear();
}

void CoreService::processRequest(ParameterRequest* request) {
  LOG("put param: client=%d, id=%s, value=%s.", request->client, request->id, request->value);
  contexts.get(request->client).putParam(request->id, request->value, request->append);
}

void CoreService::processRequest(DisplayMessageRequest* request) {
  String message = contexts.get(request->client).formatMessage(request->messageId);
  LOG("draw message: id=%s, message=%s.", request->messageId, message.c_str());
  display.drawText(message.c_str());
}

void CoreService::processRequest(DisplayMenuRequest* request) {
  MenuDef def = contexts.get(request->client).getMenuDef(request->menuId);
  LOG("open menu: id=%s.", request->menuId);
  auto& dsp = display;
  auto selectedIndex = request->selectedIndex;
  runner.schedule([&dsp, def, selectedIndex]() { dsp.drawMenu(def, selectedIndex); });
}

void CoreService::processRequest(DisplayBitmapRequest* request) {
  BitmapDef def = contexts.get(request->client).getBitmapDef(request->bitmapId);
  LOG("draw bitmap: id=%s, path=%s.", request->bitmapId, def.path);
  auto& dsp = display;
  runner.schedule([&dsp, def]() { dsp.drawBitmap(def, 0, 0); });
}

void CoreService::processRequest(PlaySoundRequest* request) {
  player.play(request->folder, request->track);
}

void CoreService::processRequest(StopSoundRequest* request) {
  player.stop();
}

void CoreService::processRequest(ReadDataRequest* request) {
  DataDef def = contexts.get(request->client).getDataDef(request->dataId);
  LOG("read data: id=%s, path=%s", request->dataId, def.path);
  storage.sdAccess([&]() { DataProvider::init(storage.open(def.path)); });
  requestedStatus = StatusType::CORE_DATA;
}

/* ========================================================================= */
/*                                           status publishing (onRequest()) */
/* ========================================================================= */

void CoreService::publishStatus() {
  switch (requestedStatus) {
    case StatusType::CORE_STATUS:
      publishCoreStatus();
      break;
    case StatusType::CORE_DATA:
      // publishDataStream();
      break;
    default:
      LOG("unknown requested status: %d", requestedStatus);
      break;
  }
}

void CoreService::publishCoreStatus() {
  CoreStatus status;
  status.playerState = Player::playerState;
  PeriphericalService::sendStatus(&status, sizeof(CoreStatus));
}

void CoreService::publishDataStream() {
  DataProvider::writeBlock();
}

bool CoreService::handleRequest( BaseRequest* request) {
	if( request->type == RequestType::STATUS && ((StatusRequest*)request)->statusType == StatusType::CORE_DATA) {
		publishDataStream();
		return true;
	}
	return false;
}
