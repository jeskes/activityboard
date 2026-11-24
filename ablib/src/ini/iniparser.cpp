#if defined(AB_INIPARSER)

#include "iniparser.h"

IniParser::IniParser(char* lineBuffer, size_t lineSize)
    : _buffer(lineBuffer),
      _size(lineSize) {
}

void IniParser::parseStream(void* context, DataStream& stream, LineHandler handler) {
  if (_buffer == nullptr || _size == 0 || handler == nullptr)
    return;

  size_t index = 0;
  bool overflow = false;
  char currentSection[32] = "";

  int raw;
  while ((raw = stream.read()) != EOF) {
    char c = static_cast<char>(raw);
    if (c == '\n') {
      if (!overflow) {
        _buffer[index] = '\0';
        processLine(context, _buffer, currentSection, sizeof(currentSection), handler);
      }
      // Reset für nächste Zeile
      index = 0;
      overflow = false;
    }
    else if (c != '\r') {
      if (index < _size - 1) {
        _buffer[index++] = c;
      }
      else {
        overflow = true;
      }
    }
  }

  if (index > 0 && !overflow) {
    _buffer[index] = '\0';
    processLine(context, _buffer, currentSection, sizeof(currentSection), handler);
  }
}

void IniParser::processLine(void* context, char* line, char* currentSection, size_t sectionSize, LineHandler handler) {
  char* trimmed = trimWhitespace(line);

  if (*trimmed == '\0' || *trimmed == ';' || *trimmed == '#') {
    return;
  }

  if (*trimmed == '[') {
    char* endBracket = strchr(trimmed + 1, ']');
    if (endBracket != nullptr) {
      *endBracket = '\0';
      strncpy(currentSection, trimWhitespace(trimmed + 1), sectionSize - 1);
      currentSection[sectionSize - 1] = '\0';
    }
    return;
  }

  char* separator = strchr(trimmed, '=');
  if (separator != nullptr) {
    *separator = '\0';
    char* key = trimWhitespace(trimmed);
    char* value = trimWhitespace(separator + 1);

    if (*key != '\0') {
      handler(context, currentSection, key, value);
    }
  }
}

char* IniParser::trimWhitespace(char* str) {
  while (isspace(static_cast<unsigned char>(*str)))
    str++;
  if (*str == '\0')
    return str;

  char* end = str + strlen(str) - 1;
  while (end > str && isspace(static_cast<unsigned char>(*end)))
    end--;
  *(end + 1) = '\0';

  return str;
}
#endif
