#include "requestqueue.h"

#ifdef ARDUINO_ARCH_ESP32
static portMUX_TYPE queueMutex = portMUX_INITIALIZER_UNLOCKED;
#endif

void RequestQueue::push(int length) {
  uint8_t nextHead = (queueHead + 1) & (REQUEST_QUEUE_SIZE - 1); /* queue size must be 2^n */
  if (nextHead == queueTail) {
    LOG("request queue is full - skip this request");
    return;
  }

  if (length > REQUEST_BUFFER_SIZE) {
    length = REQUEST_BUFFER_SIZE;
  }

  volatile Request* entry = &queue[queueHead];
  entry->length = length;

  int idx = 0;
  while (idx < length && Wire.available()) {
    entry->data[idx++] = Wire.read();
  }
  while (idx < length) {
    entry->data[idx++] = 0;
  }
  while (Wire.available()) {
    Wire.read();
  }

  queueHead = nextHead;
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

  memcpy(request, const_cast<uint8_t*>(queue[queueTail].data), queue[queueTail].length);

  queueTail = (queueTail + 1) & (REQUEST_QUEUE_SIZE - 1);

#ifdef ARDUINO_ARCH_ESP32
  portEXIT_CRITICAL(&queueMutex);
#else
  interrupts();  // arduino nano (AVR)
#endif

  return true;
}