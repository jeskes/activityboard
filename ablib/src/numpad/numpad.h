#pragma once

#include <I2CKeyPad.h>

class ActivityNumpadClient {
 private:
  I2CKeyPad keyPad;
  bool initialized;

 public:
  ActivityNumpadClient();
  void setup();
  bool available();
  char getChar();
  int16_t readString(char* buffer, uint8_t length, int16_t timeout = -1, char until = '#');
};
