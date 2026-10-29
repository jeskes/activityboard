#pragma once

#include <Adafruit_ILI9341.h>

#include "clientcontext.h"
#include "core/core.h"
#include "ledring.h"
#include "rotaryencoder/rotaryencoder.h"
#include "storage/storage.h"

class Display {
 public:
  Display(Storage& storage, LedRing& ledRing);

  void setup();
  void loop();

  void clear();
  void drawText(const char* text);
  void drawText(uint16_t backgroundColor, uint16_t textColor, const char* text);
  void drawMenu(MenuDef def, uint8_t selectedIndex);
  void drawBitmap(BitmapDef def, int16_t x, int16_t y);
  void drawBitmap(const char* path, int16_t x, int16_t y, uint16_t width = DEFAULT_BITMAP_WIDTH, uint16_t height = DEFAULT_BITMAP_HEIGHT);
  void setBrightness(int brightness, bool updateEncoder); /* 0 .. 255 */
  void toggleBrightness();

 private:
  Adafruit_ILI9341 tft;
  Storage& storage;
  ActivityRotaryEncoder brightnessEncoder;
  LedRing& ledRing;
  int brightness;
};
