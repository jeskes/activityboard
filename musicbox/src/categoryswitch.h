#include <PCF8574.h>
#include <base/base.h>
#include "const.h"

class CategorySwitch {
 public:
  CategorySwitch();

  void setup();
  MusicCategory loop();

  MusicCategory get();

 private:
  PCF8574 pcf8574;
};
