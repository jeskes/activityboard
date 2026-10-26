#include "storage.h"

#include "base/base.h"

#ifdef ARDUINO_ARCH_ESP32

static void resetSDCardSPI(uint8_t csPin, uint8_t mosiPin, uint8_t sckPin);

Storage::Storage()
    : sd(SD) {
}

void Storage::setup(uint8_t clk, uint8_t miso, uint8_t mosi, uint8_t cs) {
  if (clk == USE_DEFAULT_PIN) {
    clk = DEFAULT_SD_CLK;
  }
  if (miso == USE_DEFAULT_PIN) {
    miso = DEFAULT_SD_MISO;
  }
  if (mosi == USE_DEFAULT_PIN) {
    mosi = DEFAULT_SD_MOSI;
  }
  if (cs == USE_DEFAULT_PIN) {
    cs = DEFAULT_SD_CS;
  }

  LOG("setup SD card : clk=%d, miso=%d, mosi=%d, cs=%d...", clk, miso, mosi, cs);

  // delay(1000);
  // pinMode(SD_MISO, INPUT_PULLUP);

  resetSDCardSPI(cs, mosi, clk);

  SPI.begin(clk, miso, mosi, cs);
  if (!sd.begin(cs, SPI, 4000000)) {
    LOG("SD card not available.");
    return;
  }
  diagnostics();
  LOG("SD card successfully initialized.");
}

File Storage::open(const char* path) {
  File file = sd.open(path, FILE_READ, false);
  if (!file) {
    LOG("cannot load file: %s.", path);
  }
  return file;
}

bool Storage::loadJson(const char* path, JsonDocument& json) {
  File file = open(path);
  if (!file) {
    LOG("failed to open json file: path=%s.", path);
    return false;
  }

  DeserializationError result = deserializeJson(json, file);
  file.close();

  if (result == DeserializationError::Code::Ok) {
    return true;
  }

  LOG("failed to parse json: path=%s, error=%s.", path, result.c_str());

  return false;
}

void Storage::diagnostics() {
  list();
}

void Storage::list(const char* folder) {
  LOG("listing directory: %s.", folder);

  File root = sd.open(folder);
  if (!root) {
    LOG("failed to open directory.");
    return;
  }

  if (!root.isDirectory()) {
    LOG("not a directory.");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      LOG("  dir : %s", file.name());
      list(file.path());
    }
    else {
      LOG("  file: %s [%d bytes]", file.name(), file.size());
    }
    file = root.openNextFile();
  }
}

static void resetSDCardSPI(uint8_t cs, uint8_t mosi, uint8_t clk) {
  pinMode(cs, OUTPUT);
  pinMode(mosi, OUTPUT);
  pinMode(clk, OUTPUT);

  digitalWrite(cs, HIGH);
  digitalWrite(mosi, HIGH);

  for (int i = 0; i < 80; i++) {
    digitalWrite(clk, HIGH);
    delayMicroseconds(10);
    digitalWrite(clk, LOW);
    delayMicroseconds(10);
  }

  digitalWrite(cs, LOW);
}
#endif