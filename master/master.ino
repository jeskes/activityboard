#include <Arduino.h>
#include <Wire.h>
#include <ablib.h>
#include "handler.h"
#include "sound.h"
#include "requestStack.h"

static void processCycle();
static void handleRequest();
static void receiveRequest(MasterRequest request);

static MasterState state;

static const Processor processors[] = {
  processSound,
};
static const int countProcessors = sizeof(processors) / sizeof(Processor);

static const RequestHandler requestHandlers[] = {
  handlePlaySoundRequest,
  handleStopSoundRequest
};
static const int countRequestHandlers = sizeof(requestHandlers) / sizeof(RequestHandler);

void setup() {
  initActivityBoardLibrary(pushRequest);
  initializeSound(&state);
  LOG("Master Module started");
}

void loop() {
  processCycle();
  handleRequest();
  delay(200);
}

static void processCycle() {
  for (int idx = 0; idx < countProcessors; idx++) {
    processors[idx](&state);
  }
}

static void handleRequest() {
  if (isRequestAvailable()) {
    MasterRequest request = pullRequest();
    int idx = (int)request.requestType - 1;
    if (idx >= 0 && idx < countRequestHandlers) {
      requestHandlers[idx](&state, &request);
    }
  }
}
