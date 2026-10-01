#include "display.h"

#include <SdFat.h>

#define TFT_CS 5
#define TFT_DC 2
#define TFT_RST 4
#define TFT_BL 14

#define TFT_SDI 23
#define TFT_SCK 18

#define SD_CS 15

#define INITIAL_FOLDER_ID 10

SdFat SD_Fat;

DisplayService::DisplayService()
    : folderId(INITIAL_FOLDER_ID),
      tft(TFT_CS, TFT_DC, TFT_RST),
      reader(SD_Fat) {
}

void DisplayService::setup(uint8_t module) {
  PeriphericalService::setup(module);
  setupDisplay();
  setupSDCard();
}

void DisplayService::loop() {
  PeriphericalService::loop();
}

void DisplayService::setupSDCard() {
  LOG("initialize SD card...");
  if (!SD_Fat.begin(SD_CS, SD_SCK_MHZ(25))) {
    LOG("error: SD card not available.");
    displayMessage(ILI9341_RED, ILI9341_WHITE, "SD Card Error");
  }
  else {
    LOG("SD-Karte erfolgreich verbunden.");
  }
  LOG("Display Module started.");
}

void DisplayService::setupDisplay() {
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
  clearDisplay();
}

void DisplayService::diagnostics() {
  LOG("--- ILI9341 DIAGNOSE START ---");
  LOG("Display Power Mode: 0x%x", tft.readcommand8(ILI9341_RDMODE));
  LOG("MADCTL Mode:        0x%x", tft.readcommand8(ILI9341_RDMADCTL));
  LOG("Pixel Format:       0x%x", tft.readcommand8(ILI9341_RDPIXFMT));
  LOG("Image Format:       0x%x", tft.readcommand8(ILI9341_RDIMGFMT));
  LOG("Self Diagnostic:    0x%x", tft.readcommand8(ILI9341_RDSELFDIAG));
  LOG("------------------------------");
}

void DisplayService::processRequest(BaseRequest* request) {
  switch (request->type) {
    case RequestType::DISPLAY_SETUP:
      clientSetup(((SetupDisplayRequest*)request)->folderId);
      break;
    case RequestType::DISPLAY_CLEAR:
      clearDisplay();
      break;
    case RequestType::DISPLAY_MESSAGE:
      break;
    default:
      break;
  }
}

void DisplayService::publishStatus() {
  DisplayStatus status;
  PeriphericalService::sendStatus(&status, sizeof(DisplayStatus));
}

void DisplayService::clientSetup(uint16_t folderId) {
  LOG("client setup display: folder=%d", folderId);
  this->folderId = folderId;
}

void DisplayService::clearDisplay() {
  LOG("clearing");
  tft.fillScreen(ILI9341_BLACK);
  delay(100);
  tft.setTextColor(ILI9341_WHITE);
  delay(100);
  tft.setTextSize(12);
  delay(100);
  tft.setCursor(20, 20);
  delay(100);
  displayBitmap("/system/clear.bmp");
  LOG("cleared");
}

void DisplayService::displayMessage(const char* message) {
  displayMessage(ILI9341_BLACK, ILI9341_WHITE, message);
}

void DisplayService::displayMessage(uint16_t backgroundColor, uint16_t textColor, const char* text) {
  tft.fillScreen(backgroundColor);
  tft.setTextColor(textColor);
  tft.setTextSize(2);

  int16_t x = (320 - (strlen(text) * 12)) / 2;
  if (x < 10)
    x = 10;

  tft.setCursor(x, 110);
  tft.print(text);
}

void DisplayService::displayBitmap(const char* path) {
  ImageReturnCode stat = reader.drawBMP(path, tft, 0, 0);
  if (stat != IMAGE_SUCCESS) {
    LOG("error: missing bitmap %s.", path);
    displayMessage("missing bitmap");
  }
}