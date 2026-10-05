#include "storage.h"

#include "SD.h"
#include "const.h"

Storage::Storage()
    : sd(SD) {
}

void Storage::setup() {
  LOG("Storage: setup SD card...");
  if (!sd.begin(SD_CS)) {
    LOG("Storage: SD card not available.");
    return;
  }
  LOG("Storage: SD card mounted.");
}

File Storage::open(const char* path) {
  File file = sd.open(path, FILE_READ);
  if (!file) {
    LOG("Storage: cannot load file: %s.", path);
  }
  return file;
}

bool Storage::loadJson(const char* path, JsonDocument& json) {
  File file = open(path);
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

void Storage::diagnostics() {
  list();
}

void Storage::list(const char* folder) {
  LOG("Storage: listing directory: %s.", folder);

  File root = sd.open(folder);
  if (!root) {
    LOG("Storage: failed to open directory.");
    return;
  }

  if (!root.isDirectory()) {
    LOG("Storage: not a directory.");
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
