#ifndef _AB_DISPLAY_SERVICE
#define _AB_DISPLAY_SERVICE

#include <SPI.h>
#include <ablib.h>

#include "storage.h"
#include "display.h"
#include "clientcontext.h"
#include "taskrunner.h"

class DisplayService : public PeriphericalService {
 public:
  DisplayService();

  Storage storage;
  Display display;
  ClientContexts contexts;

  TaskRunner runner;

  void setup(uint8_t module) override;
  void loop() override;

  void processRequest(BaseRequest* request) override;
  
  private:
  static void publishStatus();
  
  void handleRequest(ClearDisplayRequest* request);
  void handleRequest(DisplayParameterRequest* request);
  void handleRequest(DisplayMessageRequest* request);
  void handleRequest(DisplayMenuRequest* request);
  void handleRequest(DisplayBitmapRequest* request);
};

#endif