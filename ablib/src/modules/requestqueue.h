#pragma once
#include "../base/base.h"

struct BaseRequest;

using RequestQueueHandler = bool (*)(BaseRequest* request);

struct Request {
  size_t length;
  uint8_t data[REQUEST_BUFFER_SIZE];
};

class RequestQueue {
 public:
  volatile uint8_t queueHead;
  volatile uint8_t queueTail;
  Request queue[REQUEST_QUEUE_SIZE];

  void push(int length);
  bool pull(uint8_t* request);

  void setHandler(RequestQueueHandler handler);

private:
	RequestQueueHandler handler;
};