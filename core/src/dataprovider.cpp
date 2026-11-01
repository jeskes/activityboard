#include "dataprovider.h"

#include "base/base.h"

bool DataProvider::active = false;
File DataProvider::activeFile = File();

size_t DataProvider::bytesRead = 0;
size_t DataProvider::bytesWritten = 0;
size_t DataProvider::bytesBuffered = 0;
uint8_t DataProvider::buffer[REQUEST_BUFFER_SIZE] = {0};

void DataProvider::init(File file) {
  close();
  bytesRead = 0;
  bytesWritten = 0;
  bytesBuffered = 0;
  memset(buffer, 0, REQUEST_BUFFER_SIZE);
  active = true;
  activeFile = file;
}

void DataProvider::readBuffer() {
  bytesBuffered = 1;
  buffer[0] = '\0';
  buffer[1] = '\1';
  buffer[1] = '\2';
  buffer[1] = '\3';

  if (!active || !activeFile) {
    LOG("file not active");
  }
  else if (!activeFile.available()) {
    LOG("no more data - automatically closing file");
    close();
  }
  else {
    bytesBuffered = activeFile.read(buffer, REQUEST_BUFFER_SIZE);
    bytesRead += bytesBuffered;
    if (bytesBuffered < REQUEST_BUFFER_SIZE) {
      LOG("data read: %d bytes. automatically closing file", bytesBuffered);
      buffer[bytesBuffered] = '\0';
      bytesBuffered++;
      close();
    }
    else {
      LOG("data read: %d bytes. more data possible", bytesBuffered);
    }
    DUMP(buffer, bytesBuffered);
  }
}

void DataProvider::writeBuffer() {
  if (bytesBuffered > 0) {
	// DUMP(buffer, 4);
    bytesWritten += Wire.slaveWrite(buffer, bytesBuffered);
    // LOG("write buffer: %d [read=%d, written=%d]", bytesBuffered, bytesRead, bytesWritten);
  }
  bytesBuffered = 0;
  memset(buffer, 0, REQUEST_BUFFER_SIZE);
}

void DataProvider::close() {
  if (active) {
    LOG("closing file");
    activeFile.close();
    activeFile = File();
  }
  active = false;
}
