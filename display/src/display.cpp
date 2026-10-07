#include "display.h"

#include "const.h"

static SPIClass spi(HSPI);

Display::Display(Storage& storage)
    : tft(&spi, TFT_DC, TFT_CS, TFT_RST),
      storage(storage) {
}

void Display::setup() {
  LOG("Display: setup TFT display...");
  spi.begin(TFT_SCK, TFT_MISO, TFT_MOSI, TFT_CS);
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
    if (def.options[idx]) {
      tft.setCursor(15, optionY);
      tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
      tft.print("option---------------");
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
    LOG("Display: bitmap file path is undefined.");
    return;
  }

  File file = storage.open(path);
  if (!file) {
    LOG("Display: bitmap file not found: file=%s", path);
    return;
  }

  uint16_t rowBuffer[width]; /* 2 bytes per pixel (16bit rgb565be) */
  for (int16_t row = 0; row < height; row++) {
    file.read((uint8_t*)rowBuffer, width * 2);
    tft.drawRGBBitmap(x, y + row, rowBuffer, width, 1);
  }
}

#ifdef NEVERDEF
void Display::drawBitmap(const char* path, int16_t x, int16_t y) {
  File file = storage.open(path);
  if (!file) {
    LOG("Display: bitmap file not found: file=%s", path);
    return;
  }

  if (file.read() != 'B' || file.read() != 'M') {
    LOG("Display: file is not a bitmap: file=%s", path);
    file.close();
    return;
  }

  file.seek(10);
  uint32_t dataOffset;
  file.read((uint8_t*)&dataOffset, 4);

  file.seek(18);
  int32_t width, height;
  file.read((uint8_t*)&width, 4);
  file.read((uint8_t*)&height, 4);

  uint16_t bitCount;
  file.seek(28);
  file.read((uint8_t*)&bitCount, 2);

  if (bitCount != 24) {
    Serial.println("Fehler: Bitte ein 24-Bit BMP aus Paint nutzen!");
    file.close();
    return;
  }

  // 2. Padding berechnen (BMP Zeilen muessen immer ein Vielfaches von 4 Bytes sein)
  uint32_t rowSize = (width * 3 + 3) & ~3;

  // Dynamische Buffer im RAM des ESP32 anlegen
  uint8_t readBuffer[width * 3];  // Nimmt die 24-Bit Zeile der SD-Karte auf
  uint16_t writeBuffer[width];    // Wandelt sie in 16-Bit fuers Display um

  // 3. Zeilenweise von unten nach oben zeichnen (Paint Standard)
  for (int32_t row = 0; row < height; row++) {
    // Ziel-Y-Koordinate auf dem Display berechnen
    int32_t tftY = y + (height - 1 - row);

    // Springe exakt zur gewuenschten Zeile in der Datei
    file.seek(dataOffset + (row * rowSize));
    file.read(readBuffer, width * 3);

    // 24-Bit (BGR) zu 16-Bit (RGB565) konvertieren
    for (int32_t col = 0; col < width; col++) {
      uint8_t b = readBuffer[col * 3];
      uint8_t g = readBuffer[col * 3 + 1];
      uint8_t r = readBuffer[col * 3 + 2];

      // Konvertierung in das hardwarenahe RGB565-Format
      writeBuffer[col] = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

    // Die komplette Zeile als High-Speed-Block an das Display senden
    tft.draw16bitRGBBitmap(x, tftY, writeBuffer, width, 1);
  }

  file.close();
}
#endif