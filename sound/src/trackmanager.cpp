#include "trackmanager.h"

#include <base/base.h>

#include <algorithm>

TrackManager::TrackManager(SDFS& sd)
    : sd(sd) {
}

void TrackManager::setup() {
  LOG("initialize trackmanager");
  folders.clear();
  File root = sd.open("/");
  if (!root || !root.isDirectory()) {
    return;
  }

  File dir = root.openNextFile();
  while (dir) {
    String dirName = String(dir.path());  // includs leading "/"
    if (dir.isDirectory() && !dirName.startsWith("/.")) {
      std::vector<String> files;
      File file = dir.openNextFile();
      while (file) {
        String fileName = String(file.name());
        if (!file.isDirectory() && !fileName.startsWith(".") && fileName.endsWith(".mp3")) {
          files.push_back(String(file.path()));
        }
        file.close();
        file = dir.openNextFile();
      }
      if (files.size()) {
        LOG("add trackfolder: path=%s, tracks=%d", dirName.c_str(), files.size());
        std::sort(files.begin(), files.end());
        folders[dirName] = std::move(files);
      }
    }
    dir.close();
    dir = root.openNextFile();
  }
  root.close();
}

const char* TrackManager::mapTrackPath(const char* path) const {
  if (!path) {
    return path;
  }
  const char* hashStr = strstr(path, "/#");
  if (!hashStr) {
    return path;
  }

  String folderPath;
  folderPath.concat(path, hashStr - path);
  auto it = folders.find(folderPath);
  if (it == folders.end()) {
    return nullptr;
  }

  size_t index = 0;
  const char* indexStr = hashStr + 2;
  if (*indexStr) {
    index = static_cast<size_t>(strtoul(indexStr, nullptr, 10));
  }

  index = index % it->second.size();
  return it->second[index].c_str();
}
