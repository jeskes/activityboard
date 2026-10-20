#include "numpad.h"
#include "base/base.h"

static char keymap[19] = "123A456B789C*0#DNF";  // N = no key, F = failure

ActivityNumpadClient::ActivityNumpadClient()
    : keyPad(NUMPAD_MODULE_ID) {
}

void ActivityNumpadClient::setup() {
  LOG("init numpad module.");
  if (keyPad.begin() == false) {
    LOG("numpad not found at address %d", NUMPAD_MODULE_ID);
  }
  else {
    LOG("loading keymap: %d", keymap);
    keyPad.loadKeyMap(keymap);
    LOG("numpad initialized at address %d", NUMPAD_MODULE_ID);
	initialized = true;
  }
}

bool ActivityNumpadClient::available() {
	return initialized;
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
      LOG("numpad readString timeout reached.");
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
