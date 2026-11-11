#include "storage.h"

#include "base/base.h"

#ifdef ARDUINO_ARCH_ESP32

static void resetSDCardSPI(uint8_t cs, uint8_t mosi, uint8_t miso, uint8_t clk);

Storage::Storage()
    : sd(SD) {
  sdMutex = xSemaphoreCreateMutex();
}

void Storage::setup(uint8_t clk, uint8_t miso, uint8_t mosi, uint8_t cs) {
  if (clk == DEFAULT_SD_PIN) {
    clk = DEFAULT_SD_CLK;
  }
  if (miso == DEFAULT_SD_PIN) {
    miso = DEFAULT_SD_MISO;
  }
  if (mosi == DEFAULT_SD_PIN) {
    mosi = DEFAULT_SD_MOSI;
  }
  if (cs == DEFAULT_SD_PIN) {
    cs = DEFAULT_SD_CS;
  }

  LOG("setup SD card : clk=%d, miso=%d, mosi=%d, cs=%d...", clk, miso, mosi, cs);

  resetSDCardSPI(cs, mosi, miso, clk);

  SPI.begin(clk, miso, mosi, -1);
  if (!sd.begin(cs, SPI)) {
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
      LOG("  dir : %s", file.path());
      list(file.path());
    }
    else {
      LOG("  file: %s [%d bytes]", file.path(), file.size());
    }
    file = root.openNextFile();
  }
}

static void resetSDCardSPI(uint8_t cs, uint8_t mosi, uint8_t miso, uint8_t clk) {
  // 1. Pins komplett als normale GPIOs isolieren
  pinMode(cs, OUTPUT);
  pinMode(mosi, OUTPUT);
  pinMode(clk, OUTPUT);
  pinMode(miso, INPUT_PULLUP); // Zwingt die hängende MISO-Leitung auf HIGH

  // 2. Zustand einfrieren (Karte abwählen)
  digitalWrite(cs, HIGH);
  digitalWrite(mosi, HIGH);
  digitalWrite(clk, LOW);
  delay(10);

  // 3. 100 aggressive Taktzyklen senden (Das wirft den SD-Controller aus jedem Freeze)
  for (int i = 0; i < 100; i++) {
    digitalWrite(clk, HIGH);
    delayMicroseconds(5);
    digitalWrite(clk, LOW);
    delayMicroseconds(5);
  }

  // 4. Den Bus kurz komplett "totstellen", damit Restspannungen im SD-Modul abfallen
  digitalWrite(cs, HIGH);
  digitalWrite(mosi, LOW);
  digitalWrite(clk, LOW);
  // pinMode(miso, OUTPUT);
  // digitalWrite(miso, LOW); 
  delay(50); // 50ms künstlicher "Mini-Stromausfall" auf den Datenleitungen

  // 5. Pins wieder in den korrekten IDLE-Zustand versetzen vor SPI.begin
  pinMode(miso, INPUT_PULLUP);
  digitalWrite(cs, HIGH);
  digitalWrite(mosi, HIGH);
  delay(20);
}

#endif