#include <stdarg.h>

#include "ablib.h"

ActivityLogger activityLogger;

void ActivityLogger::serialPrintln(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  serialPrintln(fmt, false, args);  // false = liegt im SRAM
  va_end(args);
}

void ActivityLogger::serialPrintln(const __FlashStringHelper* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  serialPrintln((const char*)fmt, true, args);
  va_end(args);
}

void ActivityLogger::serialPrintln(const char* fmt, bool isFlash,
                                   va_list args) {
  char buffer[PRINT_BUFFER_SIZE];
  if (isFlash) {
    vsnprintf_P(buffer, sizeof(buffer), fmt, args);
  } else {
    vsnprintf(buffer, sizeof(buffer), fmt, args);
  }
  Serial.println(buffer);
}
