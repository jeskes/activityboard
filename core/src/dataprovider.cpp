#include "dataprovider.h"

#include "base/base.h"

size_t DataProvider::bytesRead = 0;
size_t DataProvider::bytesWritten = 0;

static uint8_t* buffer = nullptr;

bool DataProvider::init(File file) {
  close();
  size_t size = file.size();
  buffer = (uint8_t*)malloc(size + 1);
  bytesRead = file.read(buffer, size);
  buffer[bytesRead] = 0; /* eof */
  bytesRead++;
  file.close();
  bytesWritten = 0;
  LOG("buffer read: len=%d", bytesRead);
  return true;
}

bool DataProvider::writeBlock() {
  if (!buffer) {
    return false;
  }

  unsigned int blockSize = min(bytesRead - bytesWritten, (unsigned int)REQUEST_BUFFER_SIZE);
  if (bytesRead > bytesWritten) {
    // DUMP(buffer + bytesWritten, blockSize);
    size_t written = Wire.slaveWrite(buffer + bytesWritten, blockSize);
    LOG("bytes written: %d", written);
    bytesWritten += written;
  }
  if (bytesRead <= bytesWritten) {
    close();
  }

  return true;
}

void DataProvider::close() {
  if (buffer) {
    LOG("free data buffer");
    free(buffer);
    buffer = nullptr;
  }
}
