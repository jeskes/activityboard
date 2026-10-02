#include <ablib.h>
#include "storage.h"


void Storage::setup() {
  LOG("Storage: setup SD card...");
  if (!fs.begin(SD_CS, SD_SCK_MHZ(25))) {
    LOG("Storage: SD card not available.");
  }
}

File32 Storage::open(const char* path, oflag_t flags) {
  File32 file = fs.open(path, flags);
  if (!file) {
    LOG("Storage: cannot load file: %s.", path);
  }
  return file;
}

bool Storage::loadJson(const char* path, JsonDocument& json) {
  File32 file = open(path, FILE_READ);
  if (!file) {
    LOG("Storage: failed to open json file: path=%s.", path);
    return false;
  }

  DeserializationError result = deserializeJson(json, file);
  file.close();

  if (result == DeserializationError::Code::Ok) {
    return true;
  }

  LOG("Storage: failed to parse json: path=%s, error=%s.", path, result.c_str());

  return false;
}
