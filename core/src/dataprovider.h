#pragma once

#include "FS.h"

class DataProvider {
 public:
  static bool init(File file);
  static bool writeBlock();

  static unsigned int bytesRead;
  static unsigned int bytesWritten;

  static void close();
};
