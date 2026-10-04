#include "display.h"

#include "const.h"

static SPIClass spi(HSPI);

Display::Display(Storage& storage)
    : tft(&spi, TFT_DC, TFT_CS, TFT_RST),
      storage(storage) {
}

void Display::setup() {
  LOG("Display: setup TFT display...");
  spi.begin(TFT_CLK, TFT_MISO, TFT_MOSI, TFT_CS);
  delay(500);
  tft.begin();
  tft.setRotation(1);
  LOG("Display: display initialized.");
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

void Display::drawBitmap(const char* path, int16_t x, int16_t y) {
  File file = storage.open(path);
  if (!file) {
    LOG("Display: cannot draw bitmap: file=%s", path);
    return;
  }

  if (file.read() != 'B' || file.read() != 'M') {
    LOG("Display: file is not a bitmap: file=%s", path);
    file.close();
    return;
  }

  file.seek(0x12);
  int32_t width = file.read() | (file.read() << 8) | (file.read() << 16) | (file.read() << 24);
  int32_t height = file.read() | (file.read() << 8) | (file.read() << 16) | (file.read() << 24);

  file.seek(0x1C);
  uint16_t depth = file.read() | (file.read() << 8);

  if (depth != 24) {
    LOG("Display: invalid bitmap color depth: file=%s, found=%d, expected=24", path, depth);
    file.close();
    return;
  }

  file.seek(0x0A);
  uint32_t dataOffset = file.read() | (file.read() << 8) | (file.read() << 16) | (file.read() << 24);
  file.seek(dataOffset);

  // bottom-up
  int rowSize = (width * 3 + 3) & ~3;
  uint8_t sbuf[width * 3];

  tft.startWrite();
  for (int i = 0; i < height; i++) {
    uint32_t pos = dataOffset + (height - 1 - i) * rowSize;
    file.seek(pos);
    file.read(sbuf, sizeof(sbuf));

    for (int j = 0; j < width; j++) {
      uint8_t b = sbuf[j * 3];
      uint8_t g = sbuf[j * 3 + 1];
      uint8_t r = sbuf[j * 3 + 2];
      uint16_t color = tft.color565(r, g, b);
      tft.writePixel(x + j, y + i, color);
    }
  }
  tft.endWrite();
  file.close();
}