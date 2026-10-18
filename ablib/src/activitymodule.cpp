
#include "ablib.h"

#ifdef ARDUINO_ARCH_ESP32
#else
#include <avr/sleep.h>
#endif

void GadgetBase::setup(bool sleep, uint8_t wakeupPin) {
  if (sleep) {
    enterSleep(wakeupPin);
  }
  else {
	  delay(2000);
  }
  activityBoard.setup();
  LOG("GadgetBase: activity board initialized.");
}

#ifdef ARDUINO_ARCH_ESP32
void GadgetBase::enterSleep(uint8_t wakeupPin) {
  // TODO
}
#else
ISR(PCINT0_vect) {
}  // Pins 8-13
ISR(PCINT1_vect) {
}  // Pins A0-A5
ISR(PCINT2_vect) {
}  // Pins 0-7

void GadgetBase::enterSleep(uint8_t wakeupPin) {
  pinMode(wakeupPin, INPUT_PULLUP);
  delay(10);

  byte backup_ADCSRA = ADCSRA;
  ADCSRA &= ~(1 << ADEN);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();

  // ZENTRALE ROUTINE: Manuelle und lesbare Zuweisung je nach Pin
  if (wakeupPin >= 8 && wakeupPin <= 13) {
    PCICR |= (1 << PCIE0);             // Gruppe für Pins 8-13 aktivieren
    PCMSK0 |= (1 << (wakeupPin - 8));  // Genau den richtigen Pin aktivieren (Pin 8 = Bit 0, Pin 9 = Bit 1...)
  }
  else if (wakeupPin >= A0 && wakeupPin <= A5) {
    PCICR |= (1 << PCIE1);              // Gruppe für die analogen Pins aktivieren
    PCMSK1 |= (1 << (wakeupPin - A0));  // Genau den analogen Pin aktivieren
  }
  // (Pins 0-7 lassen wir weg, da dort deine LEDs blockiert sind)

  sleep_cpu();

  /* ----------- AUFGEWACHT ----------- */
  sleep_disable();

  // Interrupts wieder sauber aufräumen
  if (wakeupPin >= 8 && wakeupPin <= 13) {
    PCMSK0 &= ~(1 << (wakeupPin - 8));
  }
  else if (wakeupPin >= A0 && wakeupPin <= A5) {
    PCMSK1 &= ~(1 << (wakeupPin - A0));
  }

  ADCSRA = backup_ADCSRA;
}
#endif

void GadgetBase::loop() {
  scheduler.execute();
}
