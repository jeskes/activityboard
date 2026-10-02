#include "display.h"

#include "ablib.h"

Display::Display(Storage& storage)
    : tft(TFT_CS, TFT_DC, TFT_RST),
      reader(storage.fs) {
}

void Display::setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  pinMode(TFT_RST, OUTPUT);
  digitalWrite(TFT_RST, LOW);
  delay(100);
  digitalWrite(TFT_RST, HIGH);
  delay(500);

  tft.begin();
  tft.setRotation(1);

  delay(100);
  diagnostics();
  clear();
}

void Display::diagnostics() {
  LOG("--- ILI9341 DIAGNOSE START ---");
  LOG("Display Power Mode: 0x%x", tft.readcommand8(ILI9341_RDMODE));
  LOG("MADCTL Mode:        0x%x", tft.readcommand8(ILI9341_RDMADCTL));
  LOG("Pixel Format:       0x%x", tft.readcommand8(ILI9341_RDPIXFMT));
  LOG("Image Format:       0x%x", tft.readcommand8(ILI9341_RDIMGFMT));
  LOG("Self Diagnostic:    0x%x", tft.readcommand8(ILI9341_RDSELFDIAG));
  LOG("------------------------------");
}

void Display::clear() {
  LOG("Display: clearing");
  tft.fillScreen(ILI9341_BLACK);
  delay(100);
  tft.setTextColor(ILI9341_WHITE);
  delay(100);
  tft.setTextSize(12);
  delay(100);
  tft.setCursor(20, 20);
  delay(100);
  drawBitmap("/system/clear.bmp");
  LOG("Display: cleared");
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

void Display::drawBitmap(const char* path) {
  ImageReturnCode stat = reader.drawBMP(path, tft, 0, 0);
  if (stat != IMAGE_SUCCESS) {
    LOG("Display: missing bitmap %s.", path);
    drawText("Display: missing bitmap");
  }
}