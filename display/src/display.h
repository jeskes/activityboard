#ifndef _AB_TFT
#define _AB_TFT

#include <Adafruit_ILI9341.h>
#include <ablib.h>

#include "storage.h"
#include "clientcontext.h"

class Display {
 public:
  Display(Storage& storage);

  void setup();

  void clear();
  void drawText(const char* text);
  void drawText(uint16_t backgroundColor, uint16_t textColor, const char* text);
  void drawMenu(MenuDef def);
  void drawBitmap(BitmapDef def, int16_t x, int16_t y);
  void drawBitmap(const char* path, int16_t x, int16_t y, uint16_t width = DEFAULT_BITMAP_WIDTH, uint16_t height = DEFAULT_BITMAP_HEIGHT);

 private:
  Adafruit_ILI9341 tft;
  Storage& storage;
};

#endif