#pragma once
#if defined(AB_INIPARSER)

#include <Arduino.h>

#include "../modules/coreclient.h"

class IniParser {
 public:
  using LineHandler = void (*)(void* context, const char* section, const char* key, const char* value);

  IniParser(char* lineBuffer, size_t lineSize);
  void parseStream(void* context, DataStream& stream, LineHandler handler);

 private:
  char* _buffer;
  size_t _size;

  void processLine(void* context, char* line, char* currentSection, size_t sectionSize, LineHandler handler);
  char* trimWhitespace(char* str);
};

#endif
