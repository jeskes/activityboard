#ifndef _AB_DISPLAY_SERVICE
#define _AB_DISPLAY_SERVICE

#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_ImageReader.h>
#include <ablib.h>

#include "SPI.h"

class DisplayService : public PeriphericalService {
 public:
  DisplayService();

  Adafruit_ILI9341 tft;
  Adafruit_ImageReader reader;

  void setup(uint8_t module) override;
  void loop() override;

  void processRequest(BaseRequest* request) override;
  void publishStatus() override;

  void clientSetup(uint16_t folderId);
  void clearDisplay();
  void displayMessage(const char* message);
  void displayMessage(uint16_t backgroundColor, uint16_t textColor, const char* text);
  void displayBitmap(const char* path);

 private:
  uint16_t folderId;
  void setupSDCard();
  void setupDisplay();
  void diagnostics();
};

#endif