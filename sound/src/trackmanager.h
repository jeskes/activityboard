#pragma once

#include <SD.h>

#include <map>
#include <vector>

class TrackManager {
 public:
  TrackManager(SDFS& sd);

  void setup();

  const char* mapTrackPath(const char* path) const;

 private:
  SDFS& sd;

  /* map folder to array of file paths */
  std::map<String, std::vector<String>> folders;
};
