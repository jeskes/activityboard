#ifndef _AB_TFT
#define _AB_TFT

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_ImageReader.h>

#include "storage.h"

#define TFT_CS 5
#define TFT_DC 2
#define TFT_RST 4
#define TFT_BL 14

class Display {
 public:
  Display(Storage& storage);

  void setup();
  void diagnostics();

  void clear();
  void drawText(const char* text);
  void drawText(uint16_t backgroundColor, uint16_t textColor, const char* text);
  void drawBitmap(const char* path);

 private:
  Adafruit_ILI9341 tft;
  Adafruit_ImageReader reader;
};

#endif