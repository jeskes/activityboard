#pragma once

#include <Adafruit_ILI9341.h>
#include <storage/storage.h>

#include "base/base.h"
#include "clientcontext.h"
#include "ledring.h"
#include "rotaryexpansion.h"

class Display {
 public:
  Display(Storage& storage, LedRing& ledRing, RotaryExpansion& rotary);

  void setup();

  void clear();
  void drawText(const char* text);
  void drawText(uint16_t backgroundColor, uint16_t textColor, const char* text);
  void drawMenu(MenuDef def, uint8_t selectedIndex);
  void drawImage(ImageDef def, int16_t x, int16_t y);
  void drawImage(const char* path, int16_t x = 0, int16_t y = 0, uint16_t width = DEFAULT_IMAGE_WIDTH, uint16_t height = DEFAULT_IMAGE_HEIGHT);
  void setBrightness(int brightness); /* 0 .. 255 */
  void toggleOff();

 private:
  Adafruit_ILI9341 tft;
  Storage& storage;
  LedRing& ledRing;
  RotaryAdapter brightnessAdapter;

  bool off;
  int brightness;

  uint16_t* buffer;
};
