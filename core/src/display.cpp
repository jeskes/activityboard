#include "display.h"

#include <ESP32Encoder.h>

#include "const.h"

static SPIClass spi(HSPI);
static ESP32Encoder encoder;

Display::Display(Storage& storage, LedRing& ledRing)
    : tft(&spi, TFT_DC, TFT_CS, TFT_RST),
      storage(storage),
      brightnessEncoder(INITIAL_BRIGHTNESS, MINIMUM_BRIGHTNESS, MAXIMUM_BRIGHTNESS, BRIGHTNESS_STEP),
      ledRing(ledRing),
      brightness(INITIAL_BRIGHTNESS) {
}

void Display::setup() {
  LOG("setup TFT display...");
  spi.begin(TFT_SCK, TFT_MISO, TFT_MOSI, -1);
  tft.begin();
  tft.setRotation(1);
  setBrightness(brightness, false);
  brightnessEncoder.setup(BRIGHTNESS_CLK, BRIGHTNESS_DT, BRIGHTNESS_SW);
  brightnessEncoder.setHandler([this](long brightness) { this->setBrightness(brightness, false); });
  LOG("display initialized.");
}

void Display::loop() {
  brightnessEncoder.loop();
  if (brightnessEncoder.pressed()) {
    toggleBrightness();
  }
}

void Display::clear() {
  drawText("display initialized");
}

void Display::drawText(const char* text) {
  drawText(ILI9341_BLACK, ILI9341_WHITE, text);
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

void Display::drawMenu(MenuDef def) {
  if (!def.title) {
    return;
  }

  tft.fillScreen(ILI9341_BLACK);

  if (def.title) {
    tft.setTextSize(2);
    tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
    tft.setCursor(10, 10);
    tft.print(def.title);
  }

  tft.setTextSize(1);
  uint16_t optionY = 80;
  uint16_t rowHeight = 40;

  for (uint8_t idx = 0; idx < MAX_MENU_OPTIONS; idx++) {
    if (def.options[idx] != nullptr) {
      tft.setCursor(15, optionY);
      tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
      tft.print(def.options[idx]);
      optionY += rowHeight;
    }
  }
}

/* always convert images with ffmpeg -i myimage.bmp -f rawvideo -pix_fmt rgb565be myimage.raw */

void Display::drawBitmap(BitmapDef def, int16_t x, int16_t y) {
  drawBitmap(def.path, x, y, def.width, def.height);
}

void Display::drawBitmap(const char* path, int16_t x, int16_t y, uint16_t width, uint16_t height) {
  if (!path || !*path) {
    LOG("bitmap file path is undefined.");
    return;
  }

  File file = storage.open(path);
  if (!file) {
    LOG("bitmap file not found: file=%s", path);
    return;
  }

  LOG("bitmap found - draw %s.", path);

  uint16_t rowBuffer[width]; /* 2 bytes per pixel (16bit rgb565be) */
  for (int16_t row = 0; row < height; row++) {
    file.read((uint8_t*)rowBuffer, width * 2);
    tft.drawRGBBitmap(x, y + row, rowBuffer, width, 1);
  }
}

void Display::setBrightness(int value, bool updateEncoder) {
  brightness = max(0, min(value, 255));
  LOG("set brightness: %d", brightness);
  analogWrite(TFT_BL, brightness);
  ledRing.updateBacklight(brightness);
  if (updateEncoder) {
    brightnessEncoder.setValue(value);
  }
}

void Display::toggleBrightness() {
  LOG("toggle brightness");
  if (brightness >= MAXIMUM_BRIGHTNESS) {
    setBrightness(MINIMUM_BRIGHTNESS, true);
  }
  else if (brightness <= MINIMUM_BRIGHTNESS) {
    setBrightness(MAXIMUM_BRIGHTNESS, true);
  }
  else {
    setBrightness(MINIMUM_BRIGHTNESS,true);
  }
}
