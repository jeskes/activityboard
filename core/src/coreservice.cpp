#include "coreservice.h"

#include "const.h"
#include "dataprovider.h"

static uint8_t statusSerial = 0;
static MaintenanceController::Configuration maintenanceConfiguration = {.hostname = "core"};

CoreService::CoreService()
    : display(storage, ledRing, rotary),
      contexts(storage),
      wifiController(contexts),
      soundController(ledRing, rotary),
      maintenanceController(maintenanceConfiguration) {
  coreControl.clear();
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
  gadgetSwitcher.setup();

  ledRing.setup();
  storage.setup(SD_CLK, SD_MISO, SD_MOSI, SD_CS);
  display.setup();
  runner.setup();
  soundController.setup();
  wifiController.setup();

  soundController.onStateChange([&]() {
    LOG("player state changed");
    fireCoreStatusUpdate();
  });

  wifiController.setHandler([&]() {
    LOG("wifi state changed");
    fireCoreStatusUpdate();
  });

  extras.addHandler(EXTRAS_STOP_ALL_BUTTON, [&]() { stopAll(); });
  extras.addHandler(EXTRAS_WIFI_BUTTON, [&]() { startStopWifi(); });
  extras.addHandler(EXTRAS_MAINENANCE_BUTTON, [&]() { startStopMaintenance(); });

  pinMode(CORE_STATUS_IRQ, OUTPUT);
  digitalWrite(CORE_STATUS_IRQ, HIGH);

  LOG("Display Service started.");
}

void CoreService::loop() {
  PeriphericalService::loop();
  soundController.loop();
  rotary.loop();
  extras.loop();
  gadgetSwitcher.loop();
  wifiController.loop();
  maintenanceController.loop();
  scheduler.execute();

  if (lastHeartbeat && millis() > lastHeartbeat + CORECLIENT_DISCONNECTED_TIMEOUT) {
    stopAll();
    lastHeartbeat = 0;
  }

  extras.showLed(EXTRAS_WIFI_LED, wifiController.isRunning());
  extras.showLed(EXTRAS_MAINENANCE_LED, maintenanceController.isRunning());
}

void CoreService::stopAll() {
  LOG("stop all");
  gadgetSwitcher.allOff();
  display.drawImage(STARTUP_IMAGE);
  soundController.stop();
  coreControl += CoreControl::STOP_ALL;
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
    case RequestType::PLAY_TRACK:
      processRequest((PlayTrackRequest*)request);
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
  coreControl.clear();
  ImageDef def = contexts.get(request->client).getImageDef(request->startupImage);
  LOG("draw startup image: id=%s, path=%s.", request->startupImage, def.path);
  auto& dsp = display;
  runner.schedule([&dsp, def]() { dsp.drawImage(def, 0, 0); });
  wifiController.configureClient(request->client);
  soundController.stop();
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
  runner.schedule([&dsp, def]() { dsp.drawImage(def); });
}

void CoreService::processRequest(PlayTrackRequest* request) {
  TrackDef def = contexts.get(request->client).getTrackDef(request->path);
  LOG("play track: path=%s, real=%s", request->path, def.path);
  if (*def.path) {
    soundController.play(def.path);
    if (request->control.has(SoundControl::DISPLAY_INFO) && def.image) {
      LOG("draw music image: %s", def.image);
      auto& dsp = display;
      runner.schedule([&dsp, def]() { dsp.drawImage(def.image); });
    }
  }
  else {
    LOG("cannot play track - no definition found");
  }
}

void CoreService::processRequest(StopSoundRequest* request) {
  soundController.stop();
}

bool CoreService::handleRequest(BaseRequest* request) {
  lastHeartbeat = millis();
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
  status.playerState = soundController.playerState;
  status.webState = wifiController.fetchState();
  status.control = coreControl;
  coreControl.clear();
  LOG("write core status: playerState=%d, webInput=0x%02X", status.playerState, status.webState.input);
  return writeStatus(&status, sizeof(CoreStatus));
}

void CoreService::startStopWifi() {
  if (wifiController.isRunning()) {
    wifiController.stop();
  }
  else {
    if (maintenanceController.isRunning()) {
      maintenanceController.stop();
      soundController.stopMaintenance();
    }
    wifiController.start();
  }
}

void CoreService::startStopMaintenance() {
  if (maintenanceController.isRunning()) {
    maintenanceController.stop();
    soundController.stopMaintenance();
  }
  else {
    if (wifiController.isRunning()) {
      wifiController.stop();
    }
    maintenanceController.start();
    soundController.startMaintenance();
  }
}
