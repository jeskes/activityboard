#include <PCF8574.h>

class CategorySwitch {
 public:
  CategorySwitch();

  void setup();

  uint8_t get();

 private:
  PCF8574 pcf8574;
};
