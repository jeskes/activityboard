#include "trackmanager.h"

#include <base/base.h>

#include <algorithm>

TrackManager::TrackManager(SDFS& sd)
    : sd(sd) {
}

void TrackManager::setup() {
  LOG("initialize trackmanager");
  folders.clear();
  initFolder("/");
}

void TrackManager::initFolder(const char* path) {
  File folder = sd.open(path);
  if (!folder || !folder.isDirectory()) {
    return;
  }

  std::vector<String> files;
  File file = folder.openNextFile();
  while (file) {
    String fileName = file.name();
    bool isDir = file.isDirectory();
    String fullPath = file.path();
    file.close();

    if (!fileName.startsWith(".")) {
      if (isDir) {
        initFolder(fullPath.c_str());
      } else {
        size_t len = fileName.length();
        if (len >= 4 && strcasecmp(fileName.c_str() + len - 4, ".mp3") == 0) {
          files.push_back(std::move(fullPath));
        }
      }
    }
    file = folder.openNextFile();
  }
  folder.close();

  if (!files.empty()) {
    LOG("add trackfolder: path=%s, tracks=%d", path, files.size());
    std::sort(files.begin(), files.end());
    folders[path] = std::move(files);
  }
}

const char* TrackManager::mapTrackPath(const char* path) const {
  if (!path) {
    return path;
  }
  const char* hashStr = strstr(path, "/#");
  if (!hashStr) {
    size_t len = strlen(path);
    if (len > 4 && !strcmp(path + (len - 4), ".mp3")) {
      return path;
    }
    static char buffer[PATH_BUFFER_SIZE];
    strcpy(buffer, path);
    strcat(buffer, ".mp3");
    return buffer;
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
