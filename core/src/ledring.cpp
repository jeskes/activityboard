#include "ledring.h"

#include "base/base.h"
#include "const.h"

LedRing::LedRing()
    : rings(LEDRING_LED_COUNT, LEDRING_DI, NEO_GRB + NEO_KHZ800) {
}

void LedRing::setup() {
    LOG("initialize ledring");
  if (!rings.begin()) {
    LOG("cannot begin ledring");
  }
  rings.setBrightness(LEDRING_BRIGHTNESS);
  rings.show();

}

void LedRing::updateVolume(int volume, bool mute) {
  LOG("update volume: vol=%d, mute=%d", volume, mute);
  if (mute) {
    for (int i = 0; i < 8; i++) {
      rings.setPixelColor(i, rings.Color(150, 0, 0));  // Rot bei Mute
    }
    rings.show();
    return;
  }

  int ledsToLight = map(volume, 0, MAXIMUM_VOLUME, 0, 8);

  for (int i = 0; i < (LEDRING_LED_COUNT / LEDRING_RING_COUNT); i++) {
    if (!i || i < ledsToLight) {
      uint8_t r = map(i, 0, 7, 0, 200);
      uint8_t g = map(i, 0, 7, 150, 50);
      uint8_t b = map(i, 0, 7, 200, 0);
      rings.setPixelColor(i, rings.Color(r, g, b));
    }
    else {
      rings.setPixelColor(i, rings.Color(0, 0, 0));
    }
  }
  rings.show();
}

void LedRing::updateBacklight(int brightness) {
  int ledsToLight = map(brightness, MINIMUM_BRIGHTNESS, MAXIMUM_BRIGHTNESS, 0, 8);
  LOG("update brightness: brightness=%d, leds=%d", brightness, ledsToLight);
  for (int i = 0; i < (LEDRING_LED_COUNT / LEDRING_RING_COUNT); i++) {
    int ledIndex = i + (LEDRING_LED_COUNT / LEDRING_RING_COUNT);
    if (i < ledsToLight) {
      rings.setPixelColor(ledIndex, rings.Color(150, 130, 100));/* warmwhite */
    }
    else {
      rings.setPixelColor(ledIndex, rings.Color(0, 0, 0));
    }
  }
  rings.show();
}
