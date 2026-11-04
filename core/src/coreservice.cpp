#include "coreservice.h"

#include "const.h"
#include "dataprovider.h"

static uint8_t statusSerial = 0;
static unsigned long nextplayback = 0;
static bool nextBitmap = false;

CoreService::CoreService()
    : player(storage, ledRing),
      contexts(storage),
      display(storage, ledRing) {
}

void CoreService::setup(uint8_t module) {
  PeriphericalService::setup(module);

  ledRing.setup();
  storage.setup();
  display.setup();
  runner.setup();
  player.setup();

  nextplayback = millis() + 20000;

  LOG("Core Service started");
}


void CoreService::loop() {
  PeriphericalService::loop();
  player.loop();
  display.loop();

  if (millis() > nextplayback) {
    player.play(STARTUP_TRACK);
	display.drawBitmap(nextBitmap ? STARTUP_BITMAP : "/10/bitmaps/fallback.raw", 0, 0);
    nextplayback = millis() + 20000;
	nextBitmap = !nextBitmap;
  }
}

void CoreService::processRequest(BaseRequest* request) {
  LOG("process request: 0x%02X", request->type);
  switch (request->type) {
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
    default:
      break;
  }
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
  TrackDef def = contexts.get(request->client).getTrackDef(request->trackId);
  LOG("play track: id=%s, path=%s.", request->trackId, def.path);
}

void CoreService::processRequest(StopSoundRequest* request) {
  player.stop();
}

bool CoreService::handleRequest(BaseRequest* request) {
  switch (request->type) {
    case RequestType::STATUS:
      return handleRequest((StatusRequest*)request);
    case RequestType::READ_DATA:
      return handleRequest((ReadDataRequest*)request);
    default:
      break;
  }
  return false;
}

bool CoreService::handleRequest(StatusRequest* request) {
  switch (request->statusType) {
    case StatusType::CORE_STATUS:
      return writeCoreStatus();
    case StatusType::CORE_DATA:
      return DataProvider::writeBlock();
    default:
      LOG("invalid status requested: 0x%02X", request->statusType);
      break;
  }
  return false;
}

bool CoreService::handleRequest(ReadDataRequest* request) {
  DataDef def = contexts.get(request->client).getDataDef(request->dataId);
  LOG("read data: id=%s, path=%s", request->dataId, def.path);
  return storage.sdAccess([&]() { return DataProvider::init(storage.open(def.path)); });
}

bool CoreService::writeCoreStatus() {
  CoreStatus status(statusSerial++);
  status.playerState = AudioPlayer::playerState;
  return writeStatus(&status, sizeof(CoreStatus));
}
