#include "requestqueue.h"

#ifdef ARDUINO_ARCH_ESP32
static portMUX_TYPE queueMutex = portMUX_INITIALIZER_UNLOCKED;
#endif

void RequestQueue::setHandler(RequestHandler* handler) {
  this->handler = handler;
}

void RequestQueue::push(int length) {
  if (length > REQUEST_BUFFER_SIZE) {
    length = REQUEST_BUFFER_SIZE;
  }

  int idx = 0;
  uint8_t buffer[REQUEST_BUFFER_SIZE];
  while (idx < length) {
    buffer[idx++] = Wire.read();
  }
  while (idx < length) {
    buffer[idx++] = 0;
  }
  while (Wire.available()) {
    Wire.read();
  }

  if (handler && handler->handleRequest((BaseRequest*)&buffer)) {
    return;
  }

  uint8_t nextHead = (queueHead + 1) & (REQUEST_QUEUE_SIZE - 1); /* queue size must be 2^n */
  if (nextHead == queueTail) {
    LOG("request queue is full - skip this request");
    return;
  }

  Request* entry = &queue[queueHead];
  entry->length = length;
  for (int idx = 0; idx < length; idx++) {
    entry->data[idx] = buffer[idx];
  }

#ifdef ARDUINO_ARCH_ESP32
  portENTER_CRITICAL(&queueMutex);
#else
  noInterrupts();  // arduino nano (AVR)
#endif

  queueHead = nextHead;

#ifdef ARDUINO_ARCH_ESP32
  portEXIT_CRITICAL(&queueMutex);
#else
  interrupts();  // arduino nano (AVR)
#endif
}

bool RequestQueue::pull(uint8_t* request) {
  if (queueHead == queueTail) {
    return false;
  }

#ifdef ARDUINO_ARCH_ESP32
  portENTER_CRITICAL(&queueMutex);
#else
  noInterrupts();  // arduino nano (AVR)
#endif

  size_t length = queue[queueTail].length;

#ifdef ARDUINO_ARCH_ESP32
  portEXIT_CRITICAL(&queueMutex);
#else
  interrupts();  // arduino nano (AVR)
#endif

  for (int idx = 0; idx < length; idx++) {
    uint8_t b = queue[queueTail].data[idx];
    request[idx] = b;
  }

  LOG("pulled request");
  DUMP(request, length);

#ifdef ARDUINO_ARCH_ESP32
  portENTER_CRITICAL(&queueMutex);
#else
  noInterrupts();  // arduino nano (AVR)
#endif

  queueTail = (queueTail + 1) & (REQUEST_QUEUE_SIZE - 1);

#ifdef ARDUINO_ARCH_ESP32
  portEXIT_CRITICAL(&queueMutex);
#else
  interrupts();  // arduino nano (AVR)
#endif

  return true;
}
