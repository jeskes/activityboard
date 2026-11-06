#pragma once

#include "../base/base.h"

struct BaseRequest;

struct Request {
  size_t length;
  uint8_t data[REQUEST_BUFFER_SIZE];
};

class BaseRequestHandler {
 public:
  virtual bool handleRequest(BaseRequest* request) = 0;
};

class RequestQueue {
 public:
  volatile uint8_t queueHead;
  volatile uint8_t queueTail;
  Request queue[REQUEST_QUEUE_SIZE];

  void push(int length);
  bool pull(uint8_t* request);

  void setHandler(BaseRequestHandler* handler);

 private:
  BaseRequestHandler* handler;
};
