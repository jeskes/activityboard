#include <stdarg.h>

#include "ablib.h"

ActivityLogger activityLogger;

void ActivityLogger::serialPrintln(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  serialPrintln(fmt, false, args);
  va_end(args);
}

#if !defined(ARDUINO_ARCH_ESP32)
void ActivityLogger::serialPrintln(const __FlashStringHelper* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  serialPrintln((const char*)fmt, true, args);  // true = liegt im Flash (PROGMEM)
  va_end(args);
}
#endif

void ActivityLogger::serialPrintln(const char* fmt, bool isFlash, va_list args) {
  char buffer[PRINT_BUFFER_SIZE];

#if defined(ARDUINO_ARCH_ESP32)
  (void)isFlash;  // avoid unused
  vsnprintf(buffer, sizeof(buffer), fmt, args);
#else
  if (isFlash) {
    vsnprintf_P(buffer, sizeof(buffer), fmt, args);
  }
  else {
    vsnprintf(buffer, sizeof(buffer), fmt, args);
  }
#endif
  Serial.println(buffer);
}