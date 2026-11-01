#pragma once

#include "FS.h"
#include <base/base.h>

class DataProvider {
 public:
  static void init(File file);
  static void readBuffer();
  static void writeBuffer();

  static bool active;
  static File activeFile;

  static size_t bytesRead;
  static size_t bytesWritten;
  static size_t bytesBuffered;
  static uint8_t buffer[REQUEST_BUFFER_SIZE];

  static void close();
};
