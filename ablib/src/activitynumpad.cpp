#include <I2CKeyPad.h>

#include "ablib.h"

/* actually a 4x4 numpad */
static char keymap[19] = "123A456B789C*0#DNF";  // N = Keine Taste, F = Fehler

ActivityNumpadClient::ActivityNumpadClient() : keyPad(NUMPAD_MODULE_ID) {
}

void ActivityNumpadClient::setup() {
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
  if (ch != 'N' && ch != 'F') {
    while (keyPad.isPressed()) {
      delay(10);
    }
  }

  return ch;
}

int ActivityNumpadClient::readString(char* buffer, int length, int timeout, char until) {
  return 0;
}