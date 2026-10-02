#include <I2CKeyPad.h>

#include "ablib.h"

static char keymap[19] = "123A456B789C*0#DNF";  // N = no key, F = failure

ActivityNumpadClient::ActivityNumpadClient()
    : keyPad(NUMPAD_MODULE_ID) {
}

void ActivityNumpadClient::init(uint16_t clientId) {
  LOG("init numpad module : client=%d.", clientId);
  if (keyPad.begin() == false) {
    LOG("Error: numpad not found at address %d", NUMPAD_MODULE_ID);
  }
  else {
    keyPad.loadKeyMap(keymap);
    LOG("Numpad initialized at address %d", NUMPAD_MODULE_ID);
  }
}

char ActivityNumpadClient::getChar() {
  if (!keyPad.isPressed()) {
    return '\0';
  }

  char ch = keyPad.getChar();

  if (ch != 'N' && ch != 'F' && ch != '\0') {
    while (keyPad.isPressed()) {
      delay(10);
    }
    return ch;
  }

  return '\0';
}

int16_t ActivityNumpadClient::readString(char* buffer, uint8_t length, int16_t timeout, char until) {
  uint8_t idx = 0;
  uint32_t startTime = millis();

  if (length > 0) {
    buffer[0] = '\0';
  }

  while (idx < (length - 1)) {
    if (timeout > 0 && (millis() - startTime >= (uint32_t)timeout)) {
      LOG("Numpad readString timeout reached.");
      break;
    }

    char ch = getChar();

    if (ch != '\0') {
      if (ch == until) {
        break;
      }

      buffer[idx++] = ch;
      buffer[idx] = '\0';

      startTime = millis();
    }

    delay(10);
  }

  return idx;
}
