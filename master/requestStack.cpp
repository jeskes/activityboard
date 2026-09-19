#include <stdlib.h>
#include <Arduino.h>
#include <ablib.h>
#include "requestStack.h"

typedef struct StackEntry {
  StackEntry* prev;
  MasterRequest* request;
};

static MasterRequest buffer;

void pushRequest(MasterRequest request) {
  // LOG("push request to stack [%d]: id=%ld, module=%d, type=%d, context=%ld, folder=%d, track=%d", countRequests(), request.id, request.module, request.type, request.data.playSound.folder, request.data.playSound.track);
  if (request.type != RequestType::UNKNOWN) {
    memcpy(&buffer, &request, sizeof(MasterRequest));
    // LOG("request pushed to stack [%d]", countRequests());
  }
}

bool isRequestAvailable() {
  return buffer.type != RequestType::UNKNOWN;
}

int countRequests() {
  return isRequestAvailable() ? 1 : 0;
}

MasterRequest pullRequest() {
  LOG("pull request from stack [%d]", countRequests());
  MasterRequest request;
  memcpy(&request, &buffer, sizeof(MasterRequest));
  LOG("pulled request from stack [%d]: id=%ld, module=%d, type=%d, context=%ld, folder=%d, track=%d", countRequests(), request.id, request.module, request.type, request.data.playSound.folder, request.data.playSound.track);
  buffer.type = RequestType::UNKNOWN;
  return request;
}
