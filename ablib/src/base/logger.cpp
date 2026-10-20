#include <stdarg.h>

#include "base.h"

ActivityLogger activityLogger;

void ActivityLogger::print(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  print(fmt, false, args);
  va_end(args);
}
void ActivityLogger::println(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  println(fmt, false, args);
  va_end(args);
}

#if !defined(ARDUINO_ARCH_ESP32)
void ActivityLogger::print(const __FlashStringHelper* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  print((const char*)fmt, true, args);  // true = liegt im Flash (PROGMEM)
  va_end(args);
}
void ActivityLogger::println(const __FlashStringHelper* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  println((const char*)fmt, true, args);  // true = liegt im Flash (PROGMEM)
  va_end(args);
}
#endif

void ActivityLogger::print(const char* fmt, bool isFlash, va_list args) {
  char buffer[PRINT_BUFFER_SIZE + 1];

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
  Serial.print(buffer);
}

void ActivityLogger::println(const char* fmt, bool isFlash, va_list args) {
  print(fmt, isFlash, args);
  Serial.println();
}

void ActivityLogger::printLocation(const char* file, unsigned int line) {
  const char* filename = (strrchr(file, '\\') ? strrchr(file, '\\') + 1 : (strrchr(file, '/') ? strrchr(file, '/') + 1 : file));
  char formatted[PRINT_FILE_SIZE + 1];
  size_t len = strlen(filename);
  if (len > PRINT_FILE_SIZE) {
    strncpy(formatted, filename + (len - PRINT_FILE_SIZE), PRINT_FILE_SIZE);
  }
  else {
    strncpy(formatted, filename, PRINT_FILE_SIZE);
  }
  formatted[PRINT_FILE_SIZE] = '\0';
  print("%20s:%3d | ", formatted, line);
}

void ActivityLogger::dump(const unsigned char* bytes, uint8_t length) {
  static const char hex_chars[] = "0123456789abcdef";

  char buffer[PRINT_BUFFER_SIZE + 1];

  if (length > PRINT_BUFFER_SIZE / 2) {
    length = PRINT_BUFFER_SIZE / 2;
  }

  for (uint8_t i = 0; i < length; i++) {
    buffer[i * 2] = hex_chars[(bytes[i] >> 4) & 0x0F];
    buffer[i * 2 + 1] = hex_chars[bytes[i] & 0x0F];
  }
  buffer[length * 2] = '\0';

  Serial.print(F("dump["));
  Serial.print(length);
  Serial.print(F("]"));
  Serial.println(buffer);
}
