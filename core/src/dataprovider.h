#pragma once

#include "FS.h"
#include <base/base.h>

class DataProvider {
 public:
  static void init(File file);
  static void writeBlock();

  static unsigned int bytesRead;
  static unsigned int bytesWritten;

  static void close();
};
