#include <PCF8574.h>
#include <base/base.h>

class CategorySwitch {
 public:
  CategorySwitch();

  void setup();
  MusicCategory loop();

  MusicCategory get();

 private:
  PCF8574 pcf8574;
};
