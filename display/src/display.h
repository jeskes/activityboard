#ifndef _AB_TFT
#define _AB_TFT

#include <Adafruit_ILI9341.h>
#include <ablib.h>

#include "storage.h"

class Display {
 public:
  Display(Storage& storage);

  void setup();

  void clear();
  void drawText(const char* text);
  void drawText(uint16_t backgroundColor, uint16_t textColor, const char* text);
  void drawBitmap(const char* path, int16_t x, int16_t y);
  void drawMenu(JsonObject menuDef);

 private:
  Adafruit_ILI9341 tft;
  Storage& storage;
};

#endif