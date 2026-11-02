#include "dataprovider.h"

#include "base/base.h"

size_t DataProvider::bytesRead = 0;
size_t DataProvider::bytesWritten = 0;

static uint8_t* buffer = nullptr;

void DataProvider::init(File file) {
  close();
  size_t size = file.size();
  buffer = (uint8_t*)malloc(size + 1);
  bytesRead = file.read(buffer, size);
  buffer[bytesRead] = 0; /* eof */
  bytesRead++;
  file.close();
  bytesWritten = 0;
}

void DataProvider::writeBlock() {
  if (buffer) {
    unsigned int blockSize = min(bytesRead - bytesWritten, (unsigned int)REQUEST_BUFFER_SIZE);
    if (bytesRead > bytesWritten) {
      DUMP(buffer + bytesWritten, blockSize);
      bytesWritten += Wire.slaveWrite(buffer + bytesWritten, blockSize);
    }
    if (bytesRead <= bytesWritten) {
      close();
    }
  }
}

void DataProvider::close() {
  if (buffer) {
    free(buffer);
    buffer = nullptr;
  }
}
