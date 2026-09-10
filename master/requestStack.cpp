#include <stdlib.h>
#include <Arduino.h>
#include <ablib.h>
#include "requestStack.h"

typedef struct StackEntry {
  StackEntry* prev;
  MasterRequest* request;
};

static StackEntry* stack = 0;

void pushRequest(MasterRequest request) {
  // LOG("push request, module: %d, type : %d", request.moduleId, request.requestType);
  if (request.requestType != RequestType::UNKNOWN) {
    StackEntry* entry = malloc(sizeof(StackEntry));
    MasterRequest* copy = malloc(sizeof(MasterRequest));
    memcpy(copy, &request, sizeof(MasterRequest));
    entry->prev = stack;
    entry->request = copy;
    stack = entry;
  }
}

int isRequestAvailable() {
  return stack ? 1 : 0;
}

MasterRequest pullRequest() {
  MasterRequest request;
  memcpy(&request, stack->request, sizeof(MasterRequest));
  // LOG("pulled request, module: %d, type : %d", request.moduleId, request.requestType);
  StackEntry* prev = stack->prev;
  free(stack->request);
  free(stack);
  stack = prev;
  return request;
}
