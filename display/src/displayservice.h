#ifndef _AB_DISPLAY_SERVICE
#define _AB_DISPLAY_SERVICE

#include <SPI.h>
#include <ablib.h>

#include "storage.h"
#include "display.h"
#include "clientcontext.h"

class DisplayService : public PeriphericalService {
 public:
  DisplayService();

  Storage storage;
  Display display;
  ClientContext context;

  void setup(uint8_t module) override;
  void loop() override;

  void processRequest(BaseRequest* request) override;
  void publishStatus() override;

 private:
  void handleRequest(InitDisplayRequest* request);
  void handleRequest(ClearDisplayRequest* request);
  void handleRequest(DisplayParameterRequest* request);
  void handleRequest(DisplayMessageRequest* request);
  void handleRequest(DisplayBitmapRequest* request);
};

#endif