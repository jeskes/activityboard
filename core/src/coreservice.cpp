#include "coreservice.h"

#include "const.h"
#include "dataprovider.h"

static uint8_t statusSerial = 0;

CoreService::CoreService()
    : player(ledRing, rotary),
      contexts(storage),
      display(storage, ledRing, rotary),
      wifi(scheduler, extras, contexts) {
}

void CoreService::setup(uint8_t module) {
  PeriphericalService::setup(module);

  if (Wire1.begin(WIRE1_SDA, WIRE1_SCL, 400000)) {
    LOG("Wire1 initialized");
  }
  else {
    LOG("Wire1 initialization failed");
  }
  rotary.setup();
  extras.setup();
  ledRing.setup();
  storage.setup(SD_CLK, SD_MISO, SD_MOSI, SD_CS);
  display.setup();
  runner.setup();
  player.setup();
  wifi.setup();

  player.onStateChange([&]() {
    LOG("player state changed");
    fireCoreStatusUpdate();
  });

  wifi.onStateChange([&]() {
    LOG("wifi state changed");
    fireCoreStatusUpdate();
  });

  pinMode(CORE_STATUS_IRQ, OUTPUT);
  digitalWrite(CORE_STATUS_IRQ, HIGH);

  LOG("Display Service started.");
}

void CoreService::loop() {
  PeriphericalService::loop();
  player.loop();
  rotary.loop();
  extras.loop();
  scheduler.execute();
}

void CoreService::processRequest(BaseRequest* request) {
  // LOG("process request: 0x%02X", request->type);
  switch (request->type) {
    case RequestType::INIT:
      processRequest((InitCoreRequest*)request);
      break;
    case RequestType::PARAMETER:
      processRequest((ParameterRequest*)request);
      break;
    case RequestType::PLAY_MUSIC:
      processRequest((PlayMusicRequest*)request);
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
    case RequestType::DISPLAY_IMAGE:
      processRequest((DisplayImageRequest*)request);
      break;
    default:
      break;
  }
}

void CoreService::processRequest(InitCoreRequest* request) {
  ImageDef def = contexts.get(request->client).getImageDef(request->startupImage);
  LOG("draw startup image: id=%s, path=%s.", request->startupImage, def.path);
  auto& dsp = display;
  runner.schedule([&dsp, def]() { dsp.drawImage(def, 0, 0); });
  wifi.configureClient(request->client);
  player.stop();
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

void CoreService::processRequest(DisplayImageRequest* request) {
  ImageDef def = contexts.get(request->client).getImageDef(request->imageId);
  LOG("draw image: id=%s, path=%s.", request->imageId, def.path);
  auto& dsp = display;
  runner.schedule([&dsp, def]() { dsp.drawImage(def, 0, 0); });
}

void CoreService::processRequest(PlayMusicRequest* request) {
  MusicDef def = contexts.get(request->client).getMusicDef(request->musicId);
  LOG("play music: id=%s, folder=%d, track=%d", request->musicId, def.folder, def.track);
  player.play(def.folder, def.track);
}

void CoreService::processRequest(PlaySoundRequest* request) {
  player.play(request->folder, request->track);
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
  return storage.sdAccess([&]() {
    File file = storage.open(def.path);
    return DataProvider::init(file);
    file.close();
  });
}

void CoreService::fireCoreStatusUpdate() {
  LOG("fire core status change");
  digitalWrite(CORE_STATUS_IRQ, LOW);
  delayMicroseconds(10);
  digitalWrite(CORE_STATUS_IRQ, HIGH);
}

bool CoreService::writeCoreStatus() {
  CoreStatus status(statusSerial++);
  status.playerState = player.playerState;
  status.webState = wifi.fetchState();
  LOG("write core status: playerState=%d, webInput=0x%02X", status.playerState, status.webState.input);
  return writeStatus(&status, sizeof(CoreStatus));
}
