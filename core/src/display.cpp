#include "display.h"

#include "const.h"

static SPIClass spi(HSPI);

Display::Display(Storage& storage, LedRing& ledRing, RotaryExpansion& rotary)
    : storage(storage),
      brightnessAdapter(INITIAL_BRIGHTNESS, MINIMUM_BRIGHTNESS, MAXIMUM_BRIGHTNESS, BRIGHTNESS_STEP),
      ledRing(ledRing),
      brightness(INITIAL_BRIGHTNESS) {
  rotary.setAdapterRight(brightnessAdapter);
}

void Display::setup() {
  LOG("setup TFT display...");

  tft.init();
  tft.setRotation(1);

  setBrightness(brightness);
  brightnessAdapter.onPressed([&]() { toggleOff(); });
  brightnessAdapter.onValueChanged([&](long brightness) { setBrightness(brightness); });

  buffer = (uint16_t*)malloc(DISPLAY_BUFFER_SIZE);
  initFolder("/");

  LOG("display initialized.");
}

void Display::clear() {
  drawText("display initialized");
}

void Display::drawText(const char* text) {
  drawText(TFT_BLACK, TFT_WHITE, text);
}

void Display::drawText(uint16_t backgroundColor, uint16_t textColor, const char* text) {
  tft.fillScreen(backgroundColor);
  tft.setTextColor(textColor);
  tft.setTextSize(2);

  int16_t x = (320 - (strlen(text) * 12)) / 2;
  if (x < 10)
    x = 10;

  tft.setCursor(x, 110);
  tft.print(text);
}

void Display::drawMenu(MenuDef def, uint8_t selectedIndex) {
  if (!def.title) {
    return;
  }

  tft.fillScreen(TFT_BLACK);

  if (def.title) {
    tft.setTextSize(2);
    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setCursor(10, 10);
    tft.print(def.title);
  }

  tft.setTextSize(1);
  uint16_t optionY = 80;
  uint16_t rowHeight = 40;

  for (uint8_t idx = 0; idx < MAX_MENU_OPTIONS; idx++) {
    if (def.options[idx] != nullptr) {
      tft.setCursor(15, optionY);
      tft.setTextColor(idx == selectedIndex ? TFT_GREEN : TFT_WHITE, TFT_BLACK);
      tft.print(def.options[idx]);
      optionY += rowHeight;
    }
  }
}

void Display::drawImage(ImageDef def, int16_t x, int16_t y) {
  drawImage(def.path, x, y, def.width, def.height);
}

void Display::drawImage(const char* path, int16_t x, int16_t y, uint16_t width, uint16_t height) {
  if (!path || !*path) {
    LOG("image file path is undefined.");
    return;
  }

  File file = storage.open(normalizeImagePath(path));
  if (!file) {
    LOG("image file not found: file=%s", path);
    return;
  }

  tft.startWrite();
  bool ok = tft.drawJpg(&file, x, y);
  tft.endWrite();

  if (!ok) {
    LOG("failed to decode jpeg: file=%s", path);
  }

  file.close();
}

void Display::setBrightness(int value) {
  off = false;
  brightness = max(0, min(value, 255));
  LOG("set brightness: %d", brightness);
  tft.setBrightness(brightness);
  ledRing.updateBrightness(brightness, off);
}

void Display::toggleOff() {
  off = !off;
  LOG("toggle off: off=%d", off);
  tft.setBrightness(off ? 0 : brightness);
  ledRing.updateBrightness(brightness, off);
}

const char* Display::normalizeImagePath(const char* path) {
  if (!path) {
    return "";
  }

  path = mapImagePath(path);

  size_t len = strlen(path);
  if (len >= 4 && !strcasecmp(path + (len - 4), ".jpg")) {
    return path;
  }

  static char normalized[PATH_BUFFER_SIZE];
  snprintf(normalized, sizeof(normalized), "%s.jpg", path);

  return normalized;
}

const char* Display::mapImagePath(const char* path) const {
  if (!path) {
    return path;
  }
  const char* hashStr = strstr(path, "/#");
  if (!hashStr) {
    return path;
  }

  String folderPath;
  folderPath.concat(path, hashStr - path);
  auto it = folders.find(folderPath);
  if (it == folders.end()) {
    return path;
  }

  size_t index = 0;
  const char* indexStr = hashStr + 2;
  if (*indexStr) {
    index = static_cast<size_t>(strtoul(indexStr, nullptr, 10));
  }

  index = index % it->second.size();
  return it->second[index].c_str();
}

void Display::initFolder(const char* path) {
  File folder = storage.sd.open(path);
  if (!folder || !folder.isDirectory()) {
    return;
  }

  std::vector<String> files;
  File file = folder.openNextFile();
  while (file) {
    String fileName = file.name();
    bool isDir = file.isDirectory();
    String fullPath = file.path();
    file.close();

    if (!fileName.startsWith(".")) {
      if (isDir) {
        initFolder(fullPath.c_str());
      }
      else {
        size_t len = fileName.length();
        if (len >= 4 && strcasecmp(fileName.c_str() + len - 4, ".jpg") == 0) {
          files.push_back(std::move(fullPath));
        }
      }
    }
    file = folder.openNextFile();
  }
  folder.close();

  if (!files.empty()) {
    LOG("add folder: path=%s, images=%d", path, files.size());
    std::sort(files.begin(), files.end());
    folders[path] = std::move(files);
  }
}
